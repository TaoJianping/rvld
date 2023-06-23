//
// Created by tjp56 on 2023/6/17.
//

#ifndef RVLD_OUTPUTFILE_H
#define RVLD_OUTPUTFILE_H

#include <string>
#include <filesystem>
#include "Bytes.h"
#include <fstream>
#include "spdlog/spdlog.h"


class OutputFile
{
public:
    explicit OutputFile(std::filesystem::path filePath);
    void WriteBytes(Bytes& bytes);

private:
    std::filesystem::path _filePath;
};


#endif //RVLD_OUTPUTFILE_H
