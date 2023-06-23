//
// Created by tjp56 on 2023/6/22.
//

#include "gtest/gtest.h"
#include <filesystem>
#include "Utils/Bytes.h"
#include "Utils/Utils.h"

TEST(TestUtils, test_bytes)
{
    auto bytes = Bytes{};

    bytes.reserve(10);
    bytes.resize(10, std::byte{'\0'});
}

TEST(TestUtils, test_GetOutputName)
{
    EXPECT_EQ(Utils::GetOutputName(".rodata.cst8", 18), ".rodata.cst");
    EXPECT_EQ(Utils::GetOutputName(".rodata.cst4", 18), ".rodata.cst");
    EXPECT_EQ(Utils::GetOutputName(".rodata.cst16", 18), ".rodata.cst");
    EXPECT_EQ(Utils::GetOutputName(".rodata.str1.8", 50), ".rodata.str");
    EXPECT_EQ(Utils::GetOutputName(".srodata.cst8", 18), ".srodata.cst8");
    EXPECT_EQ(Utils::GetOutputName(".srodata.cst4", 18), ".srodata.cst4");
}

