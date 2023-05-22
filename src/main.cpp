#include "spdlog/spdlog.h"
#include "rvld.h"
#include "spdlog/sinks/basic_file_sink.h"



int main(int argc, char** argv)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto log_path = dirPath / "logs" / "basic.log";
    auto file_logger = spdlog::basic_logger_mt("basic_logger", log_path.c_str(), true);
    spdlog::set_default_logger(file_logger);

    Linker linker{};
    auto ctx = linker.NewContext();
//        ctx->PrintArgs(argc, argv);
    ctx->ParseArgs(argc, argv);

    if (ctx->GetMachineType() == MachineType::None)
    {
        // TODO
        ctx->SetMachineType(MachineType::RISCV64);
    }

    auto files = linker.ReadInputFiles(ctx);

//    spdlog::info("Start FillUp Objects");
    ctx->FillUpObjects(files);

    auto objs = ctx->GetObjectFiles();

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

    ctx->ResolveSymbols();
//    ctx->PrintResolveSymbolMap();

    ctx->MarkLiveObjects();

    spdlog::info("live objects size -> {}", ctx->GetLiveObjectFiles().size());



    return 0;
}