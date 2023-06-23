//
// Created by tjp56 on 2023/5/24.
//

#include "gtest/gtest.h"
#include <filesystem>
#include "Utils/InputFile.h"
#include "rvld.h"

TEST(TestOutputFile, test_basic)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
//    auto outputFile = rvld::OutputFile{};
//    outputFile.WriteTo(dirPath / "out" / "tests" / "testOutput.o");
}

TEST(TestOutputFile, test_split_content)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto file = InputFile{ dirPath / "out" / "tests" / "tmp" / "error.o" };
    EXPECT_EQ(file.Exists(), true);
    EXPECT_EQ(FileType::ELF, file.GetFileType());

    auto obj = rvld::ObjectFile{&file};

}