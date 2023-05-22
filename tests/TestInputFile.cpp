
#include <filesystem>
#include "gtest/gtest.h"
#include "Define.h"
#include "InputFile.h"
#include "Archive.h"
#include "Utils/BytesReader.h"
#include "ELFFile.h"

TEST(TestInputFile, test_elf_file)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto file = InputFile{ dirPath / "out" / "tests" / "a.o" };
    EXPECT_EQ(file.Exists(), true);
    EXPECT_EQ(FileType::ELF, file.GetFileType());
}

TEST(TestInputFile, test_ar_file)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto file = InputFile{ dirPath / "out" / "tests" / "libgcc.a" };
    EXPECT_EQ(file.Exists(), true);
    EXPECT_TRUE(file.Alive());
    auto ft = file.GetFileType();
    EXPECT_EQ(FileType::Archive, ft);

    auto ar = Archive{ &file };
    auto objFiles = ar.ReadArchiveMembers();
    EXPECT_EQ(objFiles.size(), 251);

    for (auto obj : objFiles)
    {
//        spdlog::info("Find _trampoline.o");
        auto objectFile = ELF::ObjectFile{ obj };
        auto content = objectFile.ReadSymbolTableStringTable();
        auto con = objectFile.ReadSymbolTable();
        EXPECT_FALSE(obj->Alive());
    }
}



TEST(TestInputFile, test_ar_file2)
{

    std::filesystem::path filePath{ "/usr/lib/gcc-cross/riscv64-linux-gnu/10/libgcc.a" };
    auto file = InputFile{ filePath };
    EXPECT_EQ(file.Exists(), true);
    EXPECT_TRUE(file.Alive());
    auto ft = file.GetFileType();
    EXPECT_EQ(FileType::Archive, ft);

    auto ar = Archive{ &file };
    auto objFiles = ar.ReadArchiveMembers();

    spdlog::info("objFiles size -> {}", objFiles.size());
    for (auto obj : objFiles)
    {
        auto objectFile = ELF::ObjectFile{ obj };
//        auto content = objectFile.ReadStringTable();
//        auto con = objectFile.ReadSymbolTable();
        EXPECT_FALSE(obj->Alive());
    }
}