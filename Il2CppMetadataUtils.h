#pragma once

#include "Common.h"
#include "GlobalMetadataFileInternals.h"

#include <unordered_map>

class Il2CppMetadataUtils
{
public:
    // Init

    static bool LoadMetadataFile(const char* fileName);
    static bool LoadTypeData(const char* fileName);
    static bool LoadMethodPointerData(const char* fileName);

    static void InitDefaultValue();

    // Dumper

    static const char* GetStringFromIndex(const StringIndex index);
    static const XIl2CppType* GetTypeFromTypeData(const TypeIndex index);
    static const char* GetTypeName(const XIl2CppType* type);
    static void GetMethodPointerForImage(XIl2CppImage* image);
    static uint64_t GetMethodPointerFromMethod(const XIl2CppMethod* method);

    static XIl2CppImage* GetImageFromIndex(ImageIndex index);
    static XIl2CppType* GetTypeFromIndex(TypeDefinitionIndex index);
    static XIl2CppMethod* GetMethodFromIndex(MethodIndex index);
    static XIl2CppField* GetFieldFromIndex(FieldIndex index);
    static XIl2CppParameter* GetParameterFromIndex(ParameterIndex index);

    static const XIl2CppMetadata* DumpIl2CppMetadata();

    // Print

    static void PrintImagesFromMetadata(const XIl2CppMetadata* xMetadata);
    static void PrintTypesFromImage(const XIl2CppImage* ximage);
    static void PrintMethodsFromType(const XIl2CppType* xtype);
    static void PrintFieldsFromType(const XIl2CppType* xtype);
    static const char* GetParametersFromMethod(const XIl2CppMethod* xmethod);

    // Data

    static void* s_GlobalMetadata;
    static const Il2CppGlobalMetadataHeader* s_GlobalMetadataHeader;
    static const TypeDefinitionIndex* s_TypeData;   // TypeIndex to TypeDefinitionIndex
    static void* s_MethodPointerData;

    // Cache

    static XIl2CppImage* s_ImageCache;
    static XIl2CppType* s_TypeCache;
    static XIl2CppMethod* s_MethodCache;
    static XIl2CppField* s_FieldCache;
    static XIl2CppParameter* s_ParameterCache;
};