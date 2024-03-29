#include "InputFile.h"

#include <fstream>
#include <utility>

InputFile::InputFile(const fs::path& p) :
    _filePath(p), _name(p.filename().c_str())
{
    std::ifstream file(_filePath, std::ios::binary);
    // TODO 不应该一次性载入内存的，以后有机会改
    if (file)
    {
        // 获取文件大小
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        // 创建一个缓冲区来读取文件内容
        // 这里实现的很丑
        std::vector<char> buffer(size);
        _contents.reserve(size);
        if (file.read(buffer.data(), size))
        {
            // 读取成功，处理数据
            for (auto b : buffer)
            {
                _contents.push_back(static_cast<std::byte>(b));
            }
        }
        else
        {
            spdlog::error("Read Failed");
        }

        file.close();
    };
}

FileType InputFile::GetFileType()
{
    if (_contents.empty())
    {
        return FileType::Empty;
    }
    else if (_checkMagic(ELF_MAGIC))
    {
        return FileType::ELF;
    }
    else if (_checkMagic(ARCHIVE_MAGIC))
    {
        return FileType::Archive;
    }
    else
    {
        return FileType::Unknown;
    }
}

bool InputFile::IsElfFile()
{
    if (_contents.empty())
    {
        return false;
    }

    return GetFileType() == FileType::ELF;
};

Bytes InputFile::GetContents()
{
    return _contents;
}

bool InputFile::_checkMagic(const char* magic)
{
    std::size_t len = std::strlen(magic);
    if (FileSize() < len)
        return false;

    for (int i = 0; i < len; ++i)
    {
        char a = static_cast<char>(_contents.at(i));
        char b = magic[i];
        if (a != b)
        {
            return false;
        }
    }
    return true;
}

bool InputFile::Exists()
{
    return std::filesystem::exists(_filePath);
}

size_t InputFile::FileSize()
{
    return _contents.size();
}

InputFile::InputFile(std::string name, Bytes content, InputFile* parent, bool isAlive):
    _name(std::move(name)), _contents(std::move(content)), _isAlive(isAlive), _parent(parent)
{

}
std::string InputFile::GetName()
{
    return _name;
}

ELF::ElfType InputFile::GetElfFileType()
{
    if (!IsElfFile())
    {
        spdlog::error("Not Elf File");;
        return ELF::ElfType::NONE;
    }
    auto ehdr = ELF::Elf64_Ehdr{};
    std::memcpy(&ehdr, _contents.data(), sizeof(ehdr));
    return static_cast<ELF::ElfType>(ehdr.e_type);
}

bool InputFile::IsArchiveFile()
{
    if (_contents.empty())
    {
        return false;
    }

    return GetFileType() == FileType::Archive;
}

std::span<std::byte> InputFile::GetContentView()
{
    return {_contents.data(), _contents.size()};
}

bool InputFile::Alive() const
{
    return _isAlive;
}

fs::path InputFile::GetPath()
{
    return _filePath;
}

void InputFile::SetAlive()
{
    _isAlive = true;
}

InputFile* InputFile::ParentFile()
{
    return _parent;
}

ELF::Elf64_Ehdr InputFile::ElfHeader()
{
    auto ehdr = ELF::Elf64_Ehdr{};
    std::memcpy(&ehdr, _contents.data(), sizeof(ehdr));
    return ehdr;
}
