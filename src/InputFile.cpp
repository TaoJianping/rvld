#include "InputFile.h"

#include <utility>
#include <algorithm>

InputFile::InputFile(fs::path p) :
    _filePath(std::move(p)), _name(p.filename())
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
            LOG(ERROR) << "Read Failed";
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

InputFile::InputFile(std::string name, Bytes content, InputFile* parent):
    _name(std::move(name)), _contents(std::move(content))
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
        LOG(ERROR) << "Not Elf File";
        return ELF::ElfType::ET_NONE;
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
gsl::span<std::byte> InputFile::GetContentView()
{
    return {_contents.data(), _contents.size()};
}
