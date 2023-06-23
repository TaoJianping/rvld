#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <vector>
#include <array>
#include <gsl/span>

#include "spdlog/spdlog.h"

#include "../Define.h"
#include "Bytes.h"

namespace fs = std::filesystem;

constexpr char ELF_MAGIC[] = {'\x7F', 'E', 'L', 'F'};
constexpr char ARCHIVE_MAGIC[] = "!<arch>\n";

class InputFile
{
public:
    explicit InputFile(const fs::path& p);
    InputFile(std::string name, Bytes content, InputFile* parent = nullptr, bool isAlive = true);

    bool Exists();
    bool IsElfFile();
    bool IsArchiveFile();

    size_t FileSize();
    std::string GetName();
    Bytes GetContents();
    BytesView GetContentView();
    FileType GetFileType();
    ELF::ElfType GetElfFileType();
    [[nodiscard]] bool Alive() const;
    void SetAlive();
    InputFile* ParentFile();

    fs::path GetPath();

private:
    fs::path _filePath;
    Bytes _contents{};
    InputFile* _parent = nullptr;
    std::string _name{};
    bool _isAlive = true;

    bool _checkMagic(const char* magic);

};