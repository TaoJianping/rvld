//
// Created by tjp56 on 2023/5/3.
//

#include "gtest/gtest.h"

int main(int argc, char **argv) {
    // 初始化gtest库
    ::testing::InitGoogleTest(&argc, argv);
    // 运行所有测试用例
    return RUN_ALL_TESTS();
}