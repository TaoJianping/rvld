#include "spdlog/spdlog.h"
#include "rvld.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "Utils/OutputFile.h"

int main(int argc, char** argv)
{
    std::filesystem::path dirPath{ PROJECT_DIR };
    auto log_path = dirPath / "logs" / "basic.log";
    auto file_logger = spdlog::basic_logger_mt("basic_logger", log_path.c_str(), true);
    spdlog::set_default_logger(file_logger);

    rvld::Linker ld{};
    auto ctx = ld.GetContext();
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
    ctx->CreateSyntheticSections();

    ctx->CollectOutputSections();

    ctx->ComputeSectionSizes();
    ctx->ComputeSectionHeaders();

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