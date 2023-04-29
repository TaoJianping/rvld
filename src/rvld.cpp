#include <iostream>
#include <utility>
#include "rvld.h"
namespace fs = std::filesystem;

InputFile* Linker::NewInputFile(fs::path path)
{
    auto elfFile = new InputFile{ std::move(path) };
    return elfFile;
}

InputFile* Linker::NewInputFile(const std::string& path)
{
    fs::path filePath = path;

    if (!fs::exists(filePath))
    {
        LOG(INFO) << "NOT EXIST FILE";
        return nullptr;
    }

    return new InputFile{ filePath };
}

ObjectFile* Linker::NewObjectFile(InputFile* localFile)
{
    auto objFile = new ObjectFile{ localFile };
    return objFile;
};
