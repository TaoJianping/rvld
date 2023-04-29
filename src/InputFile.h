#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <vector>
#include <array>

#include "glog/logging.h"

namespace fs = std::filesystem;
using BytesVector = std::vector<std::byte>;

class InputFile
{
public:
    explicit InputFile(fs::path p);

    bool IsElfFile();
    const BytesVector GetContents();

private:
    fs::path _filePath;
    BytesVector _contents;

    bool _checkMagic();
};