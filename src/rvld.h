#pragma once

#include <cstdint>
#include <string>
#include <filesystem>
#include <fstream>
#include <cassert>

#include <spdlog/spdlog.h>

#include "Utils/strutil.h"
#include "InputFile.h"
#include "ELFFile.h"
#include "Context.h"

class Linker
{
public:
    Linker();

    InputFile* NewInputFile(fs::path path);
    InputFile* NewInputFile(const std::string& path);
    ELF::ObjectFile* NewObjectFile(InputFile* localFile);

    Context* NewContext();
    Context* GetContext();
    void SetDefaultContext(Context* ctx);

    bool FillUpObjects(Context* ctx, std::vector<InputFile*> files);
    std::vector<InputFile*> ReadInputFiles(Context* ctx);

    bool CheckFileCompatibility(Context* ctx, InputFile* inputFile);

private:
    InputFile* FindLibrary(Context* ctx, const std::string& libName);

    Context* _defaultContext = nullptr;
};
