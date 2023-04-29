#pragma once

#include <cstdint>
#include <string>
#include <filesystem>
#include <fstream>
#include <cassert>

#include <glog/logging.h>

#include "InputFile.h"
#include "ELFFile.h"
#include "ObjectFile.h"

class Linker
{
public:
    InputFile* NewInputFile(fs::path path);
    InputFile* NewInputFile(const std::string& path);
    ObjectFile* NewObjectFile(InputFile* localFile);


private:
};
