//
// Created by tjp56 on 2024/2/10.
//

#include "gtest/gtest.h"
#include "file/InputFile.h"
#include <filesystem>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


TEST(TestInputFile, test_bytes)
{
    auto bytes = Bytes{};

    bytes.reserve(10);
    bytes.resize(10, std::byte{'\0'});
}
