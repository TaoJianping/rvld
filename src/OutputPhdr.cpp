//
// Created by tjp56 on 2023/7/4.
//

#include "rvld.h"


rvld::OutputPHdr::OutputPHdr()
{
    GetSHdr()->sh_flags = SHF_ALLOC;
    GetSHdr()->sh_addralign = 8;
}

ChunkKind rvld::OutputPHdr::GetKind()
{
    return ChunkKind::HEADER;
}

void rvld::OutputPHdr::CopyBuf(rvld::Context& ctx)
{
    auto buffer = ctx.BufferRef();
    buffer->WriteVector(GetSHdr()->sh_offset, _programHeaders);
}

void rvld::OutputPHdr::UpdateShdr(rvld::Context& ctx)
{
    _programHeaders = _CreateProgramHeaderTable(ctx);
    GetSHdr()->sh_size = _programHeaders.size() * ELF::PHDR64_SIZE;
}

std::vector<ELF::Elf64_Phdr> rvld::OutputPHdr::_CreateProgramHeaderTable(rvld::Context& ctx)
{
    std::vector<ELF::Elf64_Phdr> ret {};

    auto define = [&ret](uint64_t type, uint64_t flags, int64_t minAlign, Chunk* chunk){
        ret.emplace_back(ELF::Elf64_Phdr{});
        auto& phdr = ret.back();
        phdr.p_type = type;
        phdr.p_flags = flags;
        phdr.p_align = std::max(static_cast<uint64_t>(minAlign), chunk->GetSHdr()->sh_addralign);
        phdr.p_offset = chunk->GetSHdr()->sh_offset;
        if (chunk->GetSHdr()->sh_type == SHT_NOBITS) {
            phdr.p_filesz = 0;
        } else {
            phdr.p_filesz = chunk->GetSHdr()->sh_size;
        }
        phdr.p_vaddr = chunk->GetSHdr()->sh_addr;
        phdr.p_paddr = chunk->GetSHdr()->sh_addr;
        phdr.p_memsz = chunk->GetSHdr()->sh_size;
    };

    auto push = [&ret](Chunk* chunk){
        auto& phdr = ret.back();
        phdr.p_align = std::max(phdr.p_align, chunk->GetSHdr()->sh_addralign);
        if (chunk->GetSHdr()->sh_type == SHT_NOBITS) {
            phdr.p_filesz = chunk->GetSHdr()->sh_addr + chunk->GetSHdr()->sh_size - phdr.p_vaddr;
        }
        phdr.p_memsz = chunk->GetSHdr()->sh_addr + chunk->GetSHdr()->sh_size - phdr.p_vaddr;
    };

    define(static_cast<uint64_t>(PT_PHDR), static_cast<uint64_t>(PF_R), 8, ctx.ProgramHeaderTable());



    return ret;
}
