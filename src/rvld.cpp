#include <iostream>
#include <utility>
#include "rvld.h"
#include "Archive.h"
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
}

Context* Linker::NewContext()
{
    auto ctx = new Context{};
    return ctx;
}

/*
 * 把传入的文件都给读取出来
 * */
std::vector<InputFile*> Linker::ReadInputFiles(Context* ctx)
{
    std::vector<InputFile*> passedFiles{};

    for (auto& lib : ctx->Args.Library)
    {
        auto file = FindLibrary(ctx, lib);
        if (file == nullptr || file->GetFileType() != FileType::Archive) {
            LOG(ERROR) << "File Not Right";
            break ;
        }
        passedFiles.push_back(file);
    }

    for (auto& uc : ctx->Args.UnrecognizedOption)
    {
        if (strutil::ends_with(uc, ".o"))
        {
            passedFiles.push_back(new InputFile{ std::filesystem::path{ uc } });
            //            LOG(INFO) << "Find Unrecognized Option" << uc;
        }
    }

    return passedFiles;
}

InputFile* Linker::FindLibrary(Context* ctx, const std::string& lib)
{
    std::string libName = "lib" + lib + ".a";
    for (auto& libPath : ctx->Args.LibraryPaths)
    {
        std::filesystem::path libDirPath{ libPath };
        if (std::filesystem::exists(libDirPath / libName))
        {
            auto path = libDirPath / libName;
            return new InputFile{ path };
        }
    }

    LOG(ERROR) << "Not Find Lib";
    return nullptr;
}

bool Linker::FillUpObjects(Context* ctx, std::vector<InputFile*> files)
{
    for (auto& file : files)
    {
        if (file->IsArchiveFile())
        {
            auto af = Archive{file};
            auto members = af.ReadArchiveMembers();
            for (auto f : members)
            {
                if (file->IsElfFile() && file->GetElfFileType() == ELF::ElfType::ET_REL)
                {
                    ctx->AppendObjects(NewObjectFile(f));
                }
            }
        }
        else if (file->IsElfFile() && file->GetElfFileType() == ELF::ElfType::ET_REL)
        {
            ctx->AppendObjects(NewObjectFile(file));
        }
        else
        {
            LOG(INFO) << "Not Support Type";
        }
    }


    return true;
}
