
#include <filesystem>
#include "gtest/gtest.h"
#include "glog/logging.h"
#include "Define.h"
#include "InputFile.h"
#include "Archive.h"
#include "Utils/BytesReader.h"

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
    auto ft = file.GetFileType();
    EXPECT_EQ(FileType::Archive, ft);

    auto ar = Archive{ &file };
    auto objFiles = ar.ReadArchiveMembers();
    EXPECT_EQ(objFiles.size(), 251);
}
