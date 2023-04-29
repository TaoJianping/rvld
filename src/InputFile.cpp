#include "InputFile.h"

#include <utility>
#include <algorithm>

InputFile::InputFile(fs::path p) :
    _filePath(std::move(p))
{
    std::ifstream file(_filePath, std::ios::binary);

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
            // ...
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

bool InputFile::IsElfFile()
{
    if (_contents.empty())
    {
        return false;
    }
    return _checkMagic();
};

bool InputFile::_checkMagic()
{
    BytesVector ELF_HEADER{ std::byte(0x7F), std::byte(0x45), std::byte(0x4C), std::byte(0x46) };
    return std::equal(ELF_HEADER.begin(), ELF_HEADER.end(), _contents.begin());
}

const BytesVector InputFile::GetContents()
{
    return _contents;
}
