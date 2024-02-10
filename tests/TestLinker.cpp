//
// Created by tjp56 on 2023/5/10.
//
#include <filesystem>
#include "gtest/gtest.h"
#include "Define.h"
#include "rvld.h"
#include "utils/InputFile.h"
#include "spdlog/spdlog.h"

TEST(TestLinker, test_CheckFileCompatibility)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto file = InputFile{ dirPath / "out" / "tests" / "a.o" };
    EXPECT_EQ(file.Exists(), true);
    EXPECT_EQ(FileType::ELF, file.GetFileType());

    rvld::Context ctx{};
    ctx.Args.Emulation = MachineType::RISCV64;

    auto linker = rvld::Linker{};
    EXPECT_TRUE(linker.CheckFileCompatibility(&ctx, &file));

    ctx.Args.Emulation = MachineType::None;
    EXPECT_FALSE(linker.CheckFileCompatibility(&ctx, &file));

}

TEST(TestLinker, test_Context)
{
    auto file1 = new InputFile{ "/usr/lib/gcc-cross/riscv64-linux-gnu/10/../../../../riscv64-linux-gnu/lib/crt1.o" };
    auto file2 = new InputFile{ "/usr/lib/gcc-cross/riscv64-linux-gnu/10/crti.o" };
    auto file3 = new InputFile{ "/usr/lib/gcc-cross/riscv64-linux-gnu/10/crtbeginT.o" };
    auto file4 = new InputFile{ "out/tests/hello/a.o" };
    auto file5 = new InputFile{ "/usr/lib/gcc-cross/riscv64-linux-gnu/10/crtend.o" };
//    auto file6 = new InputFile{ "/usr/lib/gcc-cross/riscv64-linux-gnu/10/crtend.o" };

    std::vector<InputFile*> files{file1, file2, file3, file4, file5};

    rvld::Linker linker{};
    auto ctx = linker.NewContext();

    if (ctx->GetMachineType() == MachineType::None)
    {
        ctx->SetMachineType(MachineType::RISCV64);
    }

//    auto files = linker.ReadInputFiles(ctx);

    //    spdlog::info("Start FillUp Objects");
    ctx->FillUpObjects(files);
    //
    //    auto objs = ctx->GetObjectFiles();
    //
    //    for (auto obj : objs)
    //    {
    //        if (!obj->HasSymbolSection()) {
    //            spdlog::info("This file has no symbol section -> {}",  obj->GetName());
    //        }
    //
    //        if (obj->GetSourceFile()->Alive())
    //        {
    //            spdlog::info("Alive File -> {}",  obj->GetName());
    //        }
    //    }

    //    ctx->ResolveSymbols();
    //    ctx->PrintResolveSymbolMap();

    //    ctx->MarkLiveObjects();

    spdlog::info("live objects size -> {}", ctx->GetLiveObjectFiles().size());




}
