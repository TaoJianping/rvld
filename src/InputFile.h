#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <vector>
#include <array>
#include <gsl/span>

#include "glog/logging.h"

#include "Define.h"
#include "Utils/Bytes.h"

namespace fs = std::filesystem;

constexpr char ELF_MAGIC[] = {'\x7F', 'E', 'L', 'F'};
constexpr char ARCHIVE_MAGIC[] = "!<arch>\n";

class InputFile
{
public:
    explicit InputFile(fs::path p);
    InputFile(std::string name, Bytes content, InputFile* parent = nullptr);

    bool Exists();
    bool IsElfFile();
    bool IsArchiveFile();
    size_t FileSize();
    std::string GetName();
    Bytes GetContents();
    gsl::span<std::byte> GetContentView();
    FileType GetFileType();
    ELF::ElfType GetElfFileType();

private:
    fs::path _filePath;
    Bytes _contents{};
    InputFile* _parent = nullptr;
    std::string _name{};

    bool _checkMagic(const char* magic);

};