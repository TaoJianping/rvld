#pragma once

#include <cstdint>
#include <string>
#include <filesystem>
#include <fstream>
#include <cassert>

#include <glog/logging.h>

#include "Utils/strutil.h"
#include "InputFile.h"
#include "ELFFile.h"
#include "ObjectFile.h"
#include "Context.h"

class Linker
{
public:
    InputFile* NewInputFile(fs::path path);
    InputFile* NewInputFile(const std::string& path);
    ObjectFile* NewObjectFile(InputFile* localFile);
    Context* NewContext();

    bool FillUpObjects(Context* ctx, std::vector<InputFile*> files);
    std::vector<InputFile*> ReadInputFiles(Context* ctx);

private:
    InputFile* FindLibrary(Context* ctx, const std::string& libName);


};
