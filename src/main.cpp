#include "spdlog/spdlog.h"
#include "rvld.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "utils/file/OutputFile.h"


void InitLogs()
{
    const std::filesystem::path dirPath{ PROJECT_DIR };
    const auto log_path = dirPath / "logs" / "basic.log";
    const auto file_logger = spdlog::basic_logger_mt("basic_logger", log_path.c_str(), true);
    set_default_logger(file_logger);
}


int main(const int argc, char** argv)
{
    // 初始化 logs
    InitLogs();

    rvld::Linker ld{};
    const auto ctx = ld.GetContext();
//    ctx->PrintArgs(argc, argv);
    ctx->ParseArgs(argc, argv);

    if (ctx->GetMachineType() == MachineType::None)
    {
        // TODO
        ctx->SetMachineType(MachineType::RISCV64);
    }

    auto files = ld.ReadInputFiles(ctx);

    auto fileNumber = ctx->FillUpObjects(files);
    ctx->ResolveSymbols();
//    ctx->PrintResolveSymbolMap();
    ctx->MarkLiveObjects();
    ctx->GenerateMergedSection();
    // link the alive input sections to output sections
    ctx->LinkInputSectionToOutputSection();
    ctx->ClearUnusedObjectsAndSymbols();
//    ctx->PrintMergedSections();
    ctx->RegisterSectionPieces();
    ctx->ComputeMergedSectionSize();
    ctx->CreateSyntheticSections();

    ctx->CollectOutputSections();
    ctx->ComputeSectionSizes();
    ctx->ComputeSectionHeaders();
    ctx->SortOutputSections();

    for (auto chunk : ctx->GetChunks())
    {
        chunk->UpdateShdr(*ctx);
    }

    auto fileSize = ctx->SetOutputSectionsOffset();
    spdlog::info("File Size -> {}", fileSize);
//    ctx->BufferRef()->ReSize(fileSize);
    ctx->BufferRef()->ReSize(fileSize);

    auto outputFile = OutputFile{ctx->Args.Output};

    for (auto chunk : ctx->GetChunks())
    {
        chunk->CopyBuf(*ctx);
    }

    outputFile.WriteBytes(*ctx->BufferRef());
    return 0;
}