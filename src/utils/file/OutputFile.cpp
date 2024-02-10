//
// Created by tjp56 on 2023/6/17.
//

#include "OutputFile.h"

#include <fstream>
#include <utility>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;


OutputFile::OutputFile(std::filesystem::path filePath):
    _filePath(std::move(filePath))
{
}

void OutputFile::WriteBytes(Bytes& bytes)
{
    std::ofstream outFile(_filePath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (outFile.is_open())
    {
        auto fileSize = bytes.size();
        spdlog::info("Write {} bytes to file {}", fileSize, _filePath.c_str());
        // 将数据写入文件
        outFile.write(reinterpret_cast<char*>(bytes.data()), fileSize);
        // 关闭输出文件流
        outFile.close();
        // 设置文件权限为0777
        fs::permissions(_filePath, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::group_write | fs::perms::others_read | fs::perms::others_write, fs::perm_options::replace);
    }
}
