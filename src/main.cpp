#include <cstdlib>
#include "rvld.h"
#include <span>
#include <concepts>

int main(int argc, char** argv)
{
//     google::InitGoogleLogging("rvld");
    Linker linker{};
    auto ctx = linker.NewContext();
    //    ctx->PrintArgs(argc, argv);
    ctx->ParseArgs(argc, argv);

    if (ctx->GetMachineType() == MachineType::None)
    {
        // TODO
        ctx->SetMachineType(MachineType::RISCV64);
    }

    auto files = linker.ReadInputFiles(ctx);

    ctx->FillUpObjects(files);

    auto objs = ctx->GetObjectFiles();
    LOG(INFO) << "Objs Length -> " << objs.size();
//    google::ShutdownGoogleLogging();
    return 0;
}