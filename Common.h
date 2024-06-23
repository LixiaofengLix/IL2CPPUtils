#pragma once

#include <iostream>
#include "GlobalMetadataFileInternals.h"

#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << (message) << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)

typedef struct XIl2CppField XIl2CppField;
typedef struct XIl2CppMethod XIl2CppMethod;
typedef struct XIl2CppType XIl2CppType;
typedef struct XIl2CppImage XIl2CppImage;

typedef struct XIl2CppParameter
{
    const char* name;
    const XIl2CppType* type;
} XIl2CppParameter;

typedef struct XIl2CppField
{
    const char* name;
    const XIl2CppType* type;
    const XIl2CppType* byType;

    const uint8_t* defaultValue;
} XIl2CppField;

typedef struct XIl2CppMethod
{
    const char* name;
    const XIl2CppType* byType;

    uint64_t methodPointer;

    const XIl2CppType *returnType;
    ParameterIndex parameterStart;
    uint32_t parameterCount;

    const Il2CppMethodDefinition* il2cppMethod;
} XIl2CppMethod;

typedef struct XIl2CppType
{
    const char* name;
    const XIl2CppImage* byImage;

    MethodIndex methodStart;
    uint32_t methodCount;

    FieldIndex fieldStart;
    uint32_t fieldCount;

    const Il2CppTypeDefinition* il2cppType;
} XIl2CppType;


typedef struct XIl2CppImage
{
    const char* name;

    TypeIndex typeStart;
    uint32_t typeCount;

    uint32_t methodPointerCount;
    uint64_t* methodPointer;

    const Il2CppImageDefinition* il2cppImage;
} XIl2CppImage;

typedef struct XIl2CppMetadata
{
    uint32_t imageCount;
} XIl2CppMetadata;


typedef struct MethodPointerDataEntry
{
    uint32_t nameIndex;
    uint32_t count;
    uint32_t dataIndex;
} MethodPointerDataEntry;


typedef struct MethodPointerDataHeader
{
    uint32_t moduleCount;
    uint32_t nameOffset;
    uint32_t dataOffset;

    const MethodPointerDataEntry* entries;

} MethodPointerDataHeader;
