//
// Created by tjp56 on 2023/5/9.
//
#include <filesystem>
#include "gtest/gtest.h"
#include "Define.h"
#include "Utils/InputFile.h"
#include "spdlog/spdlog.h"
#include "rvld.h"


TEST(TestObjectFile, test_1)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto file = InputFile{ dirPath / "out" / "tests" / "a.o" };
    EXPECT_EQ(file.Exists(), true);
    EXPECT_EQ(FileType::ELF, file.GetFileType());
}

TEST(TestObjectFile, test_GetMachineType)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto file = InputFile{ dirPath / "out" / "tests" / "a.o" };
    EXPECT_EQ(file.Exists(), true);
    EXPECT_EQ(FileType::ELF, file.GetFileType());

    auto obj = rvld::ObjectFile{&file};

    auto type = obj.GetMachineType();
    EXPECT_EQ(type, MachineType::RISCV64);
}

TEST(TestObjectFile, test_Symbols)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto file = InputFile{ dirPath / "out" / "tests" / "a.o" };
    EXPECT_EQ(file.Exists(), true);
    EXPECT_EQ(FileType::ELF, file.GetFileType());

    auto obj = rvld::ObjectFile{&file};
    EXPECT_TRUE(obj.ReadSymbolTable().size() == obj.Symbols().size());
    if (obj.ReadSymbolTable().size() != obj.Symbols().size())
    {
        spdlog::error("symbol generate failed!");
        spdlog::error("obj.ReadSymbolTable().size() -> {}", obj.ReadSymbolTable().size());
        spdlog::error("obj.Symbols().size() -> {}", obj.Symbols().size());
    }

//    for (auto section : obj.GetInputSections())
//    {
//        if (section)
//        {
//            spdlog::info("section -> {}", section->SectionName());
//        }
//    }


    for (auto symbol : obj.Symbols())
    {
        if (symbol) {
            spdlog::info("symbol -> {}", symbol->Name());
        }
    }
}


