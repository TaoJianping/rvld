//
// Created by tjp56 on 2023/6/20.
//

#include <iostream>
#include <utility>
#include "rvld.h"
#include "Utils/Archive.h"

namespace fs = std::filesystem;


InputFile* rvld::Linker::NewInputFile(fs::path path)
{
    auto elfFile = new InputFile{ std::move(path) };
    return elfFile;
}

InputFile* rvld::Linker::NewInputFile(const std::string& path)
{
    fs::path filePath = path;

    if (!fs::exists(filePath))
    {
        spdlog::error("NOT EXIST FILE");
        return nullptr;
    }

    return new InputFile{ filePath };
}

rvld::ObjectFile* rvld::Linker::NewObjectFile(InputFile* localFile)
{
    if (!CheckFileCompatibility(GetContext(), localFile)) {
        spdlog::error("incompatible file type");
    }
    auto objFile = new rvld::ObjectFile{ localFile };
    return objFile;
}

rvld::Context* rvld::Linker::NewContext()
{
    auto ctx = new Context{};
    return ctx;
}

/*
 * 把传入的文件都给读取出来
 * */
std::vector<InputFile*> rvld::Linker::ReadInputFiles(Context* ctx)
{
    std::vector<InputFile*> passedFiles{};

    for (auto& lib : ctx->Args.Library)
    {
        auto file = FindLibrary(ctx, lib);
        if (file == nullptr || file->GetFileType() != FileType::Archive) {
            spdlog::error("File Not Right");
            break ;
        }
        passedFiles.push_back(file);
    }

    for (auto& uc : ctx->Args.UnrecognizedOption)
    {
        if (strutil::ends_with(uc, ".o"))
        {
            passedFiles.push_back(new InputFile{ std::filesystem::path{ uc } });
        }
    }

    return passedFiles;
}

InputFile* rvld::Linker:: FindLibrary(Context* ctx, const std::string& lib)
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

    spdlog::error("Not Find Lib");
    return nullptr;
}

bool rvld::Linker::FillUpObjects(Context* ctx, std::vector<InputFile*> files)
{
    for (auto& file : files)
    {
        if (file->IsArchiveFile())
        {
            auto af = Archive{file};
            auto members = af.ReadArchiveMembers();
            for (auto f : members)
            {
                if (file->IsElfFile() && file->GetElfFileType() == ELF::ElfType::REL)
                {
                    ctx->AppendObjects(NewObjectFile(f));
                }
            }
        }
        else if (file->IsElfFile() && file->GetElfFileType() == ELF::ElfType::REL)
        {
            ctx->AppendObjects(NewObjectFile(file));
        }
        else
        {
            spdlog::error("Not Support Type");
        }
    }


    return true;
}

bool rvld::Linker::CheckFileCompatibility(Context* ctx, InputFile* inputFile)
{
    auto ft = inputFile->GetFileType();
    if (ft == FileType::ELF && inputFile->GetElfFileType() == ELF::ElfType::REL)
    {
        rvld::ObjectFile obj{inputFile};
        return obj.GetMachineType() == ctx->Args.Emulation;
    }

    return false;
}
void rvld::Linker::SetDefaultContext(Context* ctx)
{
    _defaultContext = ctx;
}

rvld::Context* rvld::Linker::GetContext()
{
    return _defaultContext;
}

rvld::Linker::Linker()
{
    SetDefaultContext(new Context{});
}
