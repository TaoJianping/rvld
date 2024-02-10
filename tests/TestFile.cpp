//
// Created by tjp56 on 2024/2/10.
//

#include "gtest/gtest.h"
#include "file/InputFile.h"
#include <filesystem>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


TEST(TestUtils, test_bytes)
{
    auto bytes = Bytes{};

    bytes.reserve(10);
    bytes.resize(10, std::byte{'\0'});
}



int main(int argc, char **argv) {
    // 初始化gtest库
    ::testing::InitGoogleTest(&argc, argv);

    std::filesystem::path dirPath{ PROJECT_DIR };
    auto log_path = dirPath / "logs" / "basic.log";

    auto file_logger = spdlog::basic_logger_mt("basic_logger", log_path.c_str(), true);
    spdlog::set_default_logger(file_logger);
    // 运行所有测试用例
    //    spdlog::info("-------------------------------------------");
    return RUN_ALL_TESTS();
}