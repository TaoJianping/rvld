//
// Created by tjp56 on 2023/5/24.
//

#include "rvld.h"

ELF::Elf64_Shdr* rvld::Chunk::GetSHdr()
{
    return &Shdr;
}

int64_t rvld::Chunk::GetShndx()
{
    return Shndx;
}

ChunkKind rvld::Chunk::GetKind()
{
    return ChunkKind::SYNTHETIC;
}

void rvld::Chunk::UpdateShdr(rvld::Context& ctx)
{

}

void rvld::Chunk::CopyBuf(rvld::Context& ctx)
{

}
