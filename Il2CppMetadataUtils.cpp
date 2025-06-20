#include "Il2CppMetadataUtils.h"

#include <fstream>
#include <sstream>

#include "FileLog.hpp"

void* Il2CppMetadataUtils::s_GlobalMetadata;
const TypeDefinitionIndex* Il2CppMetadataUtils::s_TypeData;
const Il2CppGlobalMetadataHeader* Il2CppMetadataUtils::s_GlobalMetadataHeader;
void* Il2CppMetadataUtils::s_MethodPointerData;

XIl2CppImage* Il2CppMetadataUtils::s_ImageCache;
XIl2CppType* Il2CppMetadataUtils::s_TypeCache;
XIl2CppMethod* Il2CppMetadataUtils::s_MethodCache;
XIl2CppField* Il2CppMetadataUtils::s_FieldCache;
XIl2CppParameter* Il2CppMetadataUtils::s_ParameterCache;

// Init

bool Il2CppMetadataUtils::LoadMetadataFile(const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (file) 
    {
        // Load global-metadata.dat
        std::streampos fileSize = file.tellg();
        void* buffer = malloc(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(static_cast<char*>(buffer), fileSize);

        s_GlobalMetadata = buffer;
        s_GlobalMetadataHeader = (const Il2CppGlobalMetadataHeader*)s_GlobalMetadata;

        // Check Format
        ASSERT(s_GlobalMetadata, "");
        ASSERT(s_GlobalMetadataHeader->sanity == 0xFAB11BAF, "Wrong Format!");
        ASSERT(s_GlobalMetadataHeader->version == 31, "Unsupported Versions!");
        ASSERT(s_GlobalMetadataHeader->stringLiteralOffset == sizeof(Il2CppGlobalMetadataHeader), "Wrong Format!");

        // Create Cache
        ASSERT(s_GlobalMetadataHeader->imagesSize % sizeof(Il2CppImageDefinition) == 0, "Il2CppImageDefinition Wrong Format!");
        uint32_t imageCount = s_GlobalMetadataHeader->imagesSize / sizeof(Il2CppImageDefinition);
        s_ImageCache = (XIl2CppImage*)malloc(imageCount * sizeof(XIl2CppImage));
        std::memset(s_ImageCache, 0, imageCount * sizeof(XIl2CppImage));

        ASSERT(s_GlobalMetadataHeader->typeDefinitionsSize % sizeof(Il2CppTypeDefinition) == 0, "Il2CppTypeDefinition Wrong Format!");
        uint32_t typeCount = s_GlobalMetadataHeader->typeDefinitionsSize / sizeof(Il2CppTypeDefinition);
        s_TypeCache = (XIl2CppType*)malloc(typeCount * sizeof(XIl2CppType));
        std::memset(s_TypeCache, 0, typeCount * sizeof(XIl2CppType));

        ASSERT(s_GlobalMetadataHeader->methodsSize % sizeof(Il2CppMethodDefinition) == 0, "Il2CppMethodDefinition Wrong Format!");
        uint32_t mtehodCount = s_GlobalMetadataHeader->methodsSize / sizeof(Il2CppMethodDefinition);
        s_MethodCache = (XIl2CppMethod*)malloc(mtehodCount * sizeof(XIl2CppMethod));
        std::memset(s_MethodCache, 0, mtehodCount * sizeof(XIl2CppMethod));

        ASSERT(s_GlobalMetadataHeader->fieldsSize % sizeof(Il2CppFieldDefinition) == 0, "Il2CppFieldDefinition Wrong Format!");
        uint32_t fieldCount = s_GlobalMetadataHeader->fieldsSize / sizeof(Il2CppFieldDefinition);
        s_FieldCache = (XIl2CppField*)malloc(fieldCount * sizeof(XIl2CppField));
        std::memset(s_FieldCache, 0, fieldCount * sizeof(XIl2CppField));

        ASSERT(s_GlobalMetadataHeader->parametersSize % sizeof(Il2CppParameterDefinition) == 0, "Il2CppParameterDefinition Wrong Format!");
        uint32_t parameterCount = s_GlobalMetadataHeader->parametersSize / sizeof(Il2CppParameterDefinition);
        s_ParameterCache = (XIl2CppParameter*)malloc(parameterCount * sizeof(XIl2CppParameter));
        std::memset(s_ParameterCache, 0, parameterCount * sizeof(XIl2CppParameter));

        ASSERT(s_ImageCache && s_TypeCache && s_MethodCache && s_FieldCache && s_ParameterCache, "Memory Allocation Failed");

        // Load types.bin
        //ASSERT(LoadTypeData("D:\\CodeRepositories\\il2cpp\\IL2CPPUtils\\TestData\\types.bin"), "Load TypeData Failed");

        //ASSERT(LoadMethodPointerData("D:\\CodeRepositories\\il2cpp\\IL2CPPUtils\\TestData\\method-pointer.bin"), "Load MethodPointerData Failed");

        // Pre Init
        InitDefaultValue();
        return true;
    }

    return false;
}

bool Il2CppMetadataUtils::LoadTypeData(const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (file) 
    {
        std::streampos fileSize = file.tellg();
        void* buffer = malloc(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(static_cast<char*>(buffer), fileSize);

        s_TypeData = (const TypeDefinitionIndex*)buffer;
        return true;
    }

    return false;
}

bool Il2CppMetadataUtils::LoadMethodPointerData(const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (file)
    {
        std::streampos fileSize = file.tellg();
        void* buffer = malloc(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(static_cast<char*>(buffer), fileSize);

        s_MethodPointerData = buffer;
        return true;
    }

    return false;
}

void Il2CppMetadataUtils::InitDefaultValue()
{
    // pre init field default value
    const Il2CppFieldDefaultValue* fieldDefaultValues = (const Il2CppFieldDefaultValue*)((const char*)s_GlobalMetadata + s_GlobalMetadataHeader->fieldDefaultValuesOffset);
    const uint32_t fieldDefaultValueCount = s_GlobalMetadataHeader->fieldDefaultValuesSize / sizeof(Il2CppFieldDefaultValue);

    for (uint32_t i = 0; i < fieldDefaultValueCount; i++)
    {
        const Il2CppFieldDefaultValue* value = fieldDefaultValues + i;
        DefaultValueDataIndex valueDataIndex = value->dataIndex;
        XIl2CppField* field = GetFieldFromIndex(value->fieldIndex);

        ASSERT(valueDataIndex >= 0 && static_cast<uint32_t>(valueDataIndex) < s_GlobalMetadataHeader->fieldAndParameterDefaultValueDataSize / sizeof(uint8_t), "FieldDefaultValueData Wrong Format!");
        const uint8_t* fieldDefaultValuesData = (const uint8_t*)((const char*)s_GlobalMetadata + s_GlobalMetadataHeader->fieldAndParameterDefaultValueDataOffset);
        field->defaultValue = fieldDefaultValuesData + valueDataIndex;
    }
}


// Dumper

const char* Il2CppMetadataUtils::GetStringFromIndex(const StringIndex index)
{
    ASSERT(index <= s_GlobalMetadataHeader->stringSize, "String Wrong Format!");
    return reinterpret_cast<const char*>(reinterpret_cast<uint8_t*>(const_cast<void*>(s_GlobalMetadata)) + s_GlobalMetadataHeader->stringOffset) + index;
}

const XIl2CppType* Il2CppMetadataUtils::GetTypeFromTypeData(const TypeIndex index)
{
    if (!s_TypeData) return NULL;
    const TypeDefinitionIndex typeIndex = s_TypeData[index];
    if (typeIndex >= 0 && static_cast<uint32_t>(typeIndex) < s_GlobalMetadataHeader->typeDefinitionsSize / sizeof(Il2CppTypeDefinition))
    {
        return GetTypeFromIndex(typeIndex);
    }
    return NULL;
}

const char* Il2CppMetadataUtils::GetTypeName(const XIl2CppType* type)
{
    if (type == NULL)
        return "<unkown>";
    return type->name;
}

void Il2CppMetadataUtils::GetMethodPointerForImage(XIl2CppImage* image)
{
    if (!s_MethodPointerData)
    {
        image->methodPointerCount = 0;
        return;
    }
    MethodPointerDataHeader* methodPointerDataHeader = (MethodPointerDataHeader*)s_MethodPointerData;
    const MethodPointerDataEntry* entries = reinterpret_cast<const MethodPointerDataEntry*>(reinterpret_cast<uint8_t*>(const_cast<void*>(s_MethodPointerData)) + 12);
    const char* nameData = reinterpret_cast<const char*>(reinterpret_cast<uint8_t*>(const_cast<void*>(s_MethodPointerData)) + methodPointerDataHeader->nameOffset);
    for (uint32_t i = 0; i < methodPointerDataHeader->moduleCount; i++)
    {
        const MethodPointerDataEntry* entry = entries + i;
        if (strcmp(image->name, nameData + entry->nameIndex) == 0)
        {
            image->methodPointerCount = entry->count;
            image->methodPointer = reinterpret_cast<uint64_t*>(reinterpret_cast<uint8_t*>(const_cast<void*>(s_MethodPointerData)) + methodPointerDataHeader->dataOffset + entry->dataIndex);
            break;
        }
    }
}

uint64_t Il2CppMetadataUtils::GetMethodPointerFromMethod(const XIl2CppMethod* method)
{
    uint32_t index = method->il2cppMethod->token & 0x00FFFFFF;
    ASSERT(index >= 0 && static_cast<uint32_t>(index) <= method->byType->byImage->methodPointerCount, "MethodPointer Wrong Format!");
    return method->byType->byImage->methodPointer[index];
}




XIl2CppImage* Il2CppMetadataUtils::GetImageFromIndex(ImageIndex index)
{
    ASSERT(index >= 0 && static_cast<uint32_t>(index) < s_GlobalMetadataHeader->imagesSize / sizeof(Il2CppImageDefinition), "Image Wrong Format!");
    const Il2CppImageDefinition* imagesDefinitions = (const Il2CppImageDefinition*)((const char*)s_GlobalMetadata + s_GlobalMetadataHeader->imagesOffset);
    const Il2CppImageDefinition* imageDefinition = imagesDefinitions + index;

    // Get From Cache
    XIl2CppImage* image = s_ImageCache + index;
    if (!image->name)
    {
        image->il2cppImage = imageDefinition;
        image->name = GetStringFromIndex(imageDefinition->nameIndex);
        image->typeStart = imageDefinition->typeStart;
        image->typeCount = imageDefinition->typeCount;
        GetMethodPointerForImage(image);

        uint32_t endIndex = image->typeStart + image->typeCount;
        for (uint32_t i = image->typeStart; i < endIndex; i++)
        {
            XIl2CppType* type = GetTypeFromIndex(i);
            type->byImage = image;
        }
    }

    return image;
}

XIl2CppType* Il2CppMetadataUtils::GetTypeFromIndex(TypeDefinitionIndex index)
{
    ASSERT(index >= 0 && static_cast<uint32_t>(index) < s_GlobalMetadataHeader->typeDefinitionsSize / sizeof(Il2CppTypeDefinition), "Type Wrong Format!");
    const Il2CppTypeDefinition* typeDefinitions = (const Il2CppTypeDefinition*)((const char*)s_GlobalMetadata + s_GlobalMetadataHeader->typeDefinitionsOffset);
    const Il2CppTypeDefinition* typeDefinition = typeDefinitions + index;

    // Get From Cache
    XIl2CppType* type = s_TypeCache + index;
    if (!type->name)
    {
        type->il2cppType = typeDefinition;
        type->name = GetStringFromIndex(typeDefinition->nameIndex);
        type->methodStart = typeDefinition->methodStart;
        type->methodCount = typeDefinition->method_count;
        type->fieldStart = typeDefinition->fieldStart;
        type->fieldCount = typeDefinition->field_count;

        if (typeDefinition->genericContainerIndex != -1)
        {
            //
        }

        uint32_t methodEndIndex = type->methodStart + type->methodCount;
        for (uint32_t i = type->methodStart; i < methodEndIndex; i++)
        {
            XIl2CppMethod* method = GetMethodFromIndex(i);
            method->byType = type;
        }

        uint32_t fieldEndIndex = type->fieldStart + type->fieldCount;
        for (uint32_t i = type->fieldStart; i < fieldEndIndex; i++)
        {
            XIl2CppField* field = GetFieldFromIndex(i);
            field->byType = type;
        }
    }
    return type;
}

XIl2CppMethod* Il2CppMetadataUtils::GetMethodFromIndex(MethodIndex index)
{
    ASSERT(index >= 0 && static_cast<uint32_t>(index) < s_GlobalMetadataHeader->methodsSize / sizeof(Il2CppMethodDefinition), "Method Wrong Format!");
    const Il2CppMethodDefinition* methodDefinitions = (const Il2CppMethodDefinition*)((const char*)s_GlobalMetadata + s_GlobalMetadataHeader->methodsOffset);
    const Il2CppMethodDefinition* methodDefinition = methodDefinitions + index;

    // Get From Cache
    XIl2CppMethod* method = s_MethodCache + index;
    if (!method->name)
    {
        method->il2cppMethod = methodDefinition;
        method->name = GetStringFromIndex(methodDefinition->nameIndex);
        //method->methodPointer = method->byType->byImage->methodPointer[methodDefinition->token & 0x00FFFFFF];

        method->returnType = GetTypeFromTypeData(methodDefinition->returnType);
        method->parameterStart = methodDefinition->parameterStart;
        method->parameterCount = methodDefinition->parameterCount;

        uint32_t paramterEndIndex = method->parameterStart + method->parameterCount;
        for (uint32_t i = method->parameterStart; i < paramterEndIndex; i++)
        {
            GetParameterFromIndex(i);
        }
    }

    return method;
}

XIl2CppField* Il2CppMetadataUtils::GetFieldFromIndex(FieldIndex index)
{
    ASSERT(index >= 0 && static_cast<uint32_t>(index) < s_GlobalMetadataHeader->fieldsSize / sizeof(Il2CppFieldDefinition), "Field Wrong Format!");
    const Il2CppFieldDefinition* fieldDefinitions = (const Il2CppFieldDefinition*)((const char*)s_GlobalMetadata + s_GlobalMetadataHeader->fieldsOffset);
    const Il2CppFieldDefinition* fieldDefinition = fieldDefinitions + index;

    // Get From Cache
    XIl2CppField* field = s_FieldCache + index;
    if (!field->name)
    {
        field->name = GetStringFromIndex(fieldDefinition->nameIndex);
        field->type = GetTypeFromTypeData(fieldDefinition->typeIndex);
    }

    return field;
}

XIl2CppParameter* Il2CppMetadataUtils::GetParameterFromIndex(ParameterIndex index)
{
    ASSERT(index >= 0 && static_cast<uint32_t>(index) < s_GlobalMetadataHeader->parametersSize / sizeof(Il2CppParameterDefinition), "Paramter Wrong Format!");
    const Il2CppParameterDefinition* parameterDefinitions = (const Il2CppParameterDefinition*)((const char*)s_GlobalMetadata + s_GlobalMetadataHeader->parametersOffset);
    const Il2CppParameterDefinition* parameterDefinition = parameterDefinitions + index;

    // Get From Cache
    XIl2CppParameter* parameter = s_ParameterCache + index;
    if (!parameter->name)
    {
        parameter->name = GetStringFromIndex(parameterDefinition->nameIndex);
        parameter->type = GetTypeFromTypeData(parameterDefinition->typeIndex);
    }

    return parameter;
}


const XIl2CppMetadata* Il2CppMetadataUtils::DumpIl2CppMetadata()
{
    XIl2CppMetadata* xMetadata = (XIl2CppMetadata *)malloc(sizeof(XIl2CppMetadata));
    ASSERT(xMetadata != NULL, "Memory Allocation Failed");

    xMetadata->imageCount = s_GlobalMetadataHeader->imagesSize / sizeof(Il2CppImageDefinition);
    for (uint32_t i = 0; i < xMetadata->imageCount; i++)
        GetImageFromIndex(i);

    return xMetadata;
}


// Print

void Il2CppMetadataUtils::PrintImagesFromMetadata(const XIl2CppMetadata* xMetadata)
{
    for (uint32_t i = 0; i < xMetadata->imageCount; i++)
    {
        const XIl2CppImage* ximage = s_ImageCache + i;
        FileLog::Out().fmt("image[%d] = %s, typeCount = %d", i, ximage->name, ximage->typeCount);
        PrintTypesFromImage(ximage);
    }
}

void Il2CppMetadataUtils::PrintTypesFromImage(const XIl2CppImage* ximage)
{
    uint32_t endIndex = ximage->typeStart + ximage->typeCount;
    for (uint32_t i = ximage->typeStart; i < endIndex; i++)
    {
        const XIl2CppType* xtype = s_TypeCache + i;
        FileLog::Out().fmt("\t------------------------------------------------------------------------------------------------------");
        FileLog::Out().fmt("\ttype[%d] = %s, fieldCount = %d,methodCount = %d", i, xtype->name, xtype->fieldCount, xtype->methodCount);
        PrintFieldsFromType(xtype);
        PrintMethodsFromType(xtype);
    }
}

void Il2CppMetadataUtils::PrintMethodsFromType(const XIl2CppType* xtype)
{
    uint32_t endIndex = xtype->methodStart + xtype->methodCount;
    for (uint32_t i = xtype->methodStart; i < endIndex; i++)
    {
        const XIl2CppMethod* xmethod = s_MethodCache + i;

        const char* ps = GetParametersFromMethod(xmethod);
        //FileLog::Out().fmt("\t\tmethod[%d] = %s %s(%s), methodPointerIndex = 0x%016llX", i, GetTypeName(xmethod->returnType), xmethod->name, GetParametersFromMethod(xmethod), GetMethodPointerFromMethod(xmethod));
        FileLog::Out().fmt("\t\tmethod[%d] = %s %s(%s)", i, GetTypeName(xmethod->returnType), xmethod->name, GetParametersFromMethod(xmethod));
    }
}

void Il2CppMetadataUtils::PrintFieldsFromType(const XIl2CppType* xtype)
{
    uint32_t endIndex = xtype->fieldStart + xtype->fieldCount;
    for (uint32_t i = xtype->fieldStart; i < endIndex; i++)
    {
        const XIl2CppField* xfield = s_FieldCache + i;

        if (xfield->defaultValue)
            // TODO 
            FileLog::Out().fmt("\t\tfield[%d] = (%s)%s = %d", i, GetTypeName(xfield->type), xfield->name, (uint32_t)*xfield->defaultValue >> 1);
        else
            FileLog::Out().fmt("\t\tfield[%d] = (%s)%s", i, GetTypeName(xfield->type), xfield->name);
    }
}

const char* Il2CppMetadataUtils::GetParametersFromMethod(const XIl2CppMethod* xmethod)
{
    std::stringstream parameterStr;
    uint32_t endIndex = xmethod->parameterStart + xmethod->parameterCount;
    for (uint32_t i = xmethod->parameterStart; i < endIndex; i++)
    {
        const XIl2CppParameter* xparameter = s_ParameterCache + i;

        if (i == xmethod->parameterStart)
            parameterStr << GetTypeName(xparameter->type) << " " << xparameter->name;
        else
            parameterStr <<", " << GetTypeName(xparameter->type) << " " << xparameter->name;
    }

    char* result = new char[parameterStr.str().length() + 1];
    std::strcpy(result, parameterStr.str().c_str()); // 复制字符串内容

    return result;
}