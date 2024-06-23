#include <iostream>
#include <string.h>

#include "Common.h"
#include "Il2CppMetadataUtils.h"
#include "GlobalMetadataFileInternals.h"

int main() 
{
    bool ret = Il2CppMetadataUtils::LoadMetadataFile("D:\\CodeRepositories\\il2cpp\\IL2CPPUtils\\TestData\\global-metadata.dat");
    if (!ret) {
        std::cerr << "Failed to open the file." << std::endl;
        return -1;
    }
    
    const XIl2CppMetadata *xMetadata =  Il2CppMetadataUtils::DumpIl2CppMetadata();
    Il2CppMetadataUtils::PrintImagesFromMetadata(xMetadata);
    // TODO
    return 0;
}