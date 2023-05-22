//
// Created by tjp56 on 2023/5/3.
//

#include "gtest/gtest.h"
#include <filesystem>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

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