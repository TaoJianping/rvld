
#include "rvld.h"

namespace fs = std::filesystem;

rvld::OutputSHdr::OutputSHdr()
{
    Name = "Section Header Table";
    Shdr.sh_addralign = 8;
}

ChunkKind rvld::OutputSHdr::GetKind()
{
    return ChunkKind::HEADER;
}

void rvld::OutputSHdr::UpdateShdr(Context& ctx)
{
    uint64_t num = 0;
    for (auto chunk : ctx.GetChunks())
    {
        auto n = chunk->GetShndx();
        if (n > 0)
        {
            num = n;
        }
    }

    Shdr.sh_size = (num + 1) * ELF::SHDR64_SIZE;
}

void rvld::OutputSHdr::CopyBuf(rvld::Context& ctx)
{
    std::vector<ELF::Elf64_Shdr> shs {};
    shs.emplace_back(ELF::Elf64_Shdr{});

    for (auto chunk : ctx.GetChunks())
    {
        if (chunk->GetShndx() > 0)
        {
            shs.emplace_back(chunk->Shdr);
        }
    }

    auto bytes = ctx.BufferRef();
    auto pos = Shdr.sh_offset;
//
    bytes->WriteVector<ELF::Elf64_Shdr>(pos, shs);
}
