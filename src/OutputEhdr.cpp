//
// Created by tjp56 on 2023/6/20.
//
#include "rvld.h"

namespace
{

}


rvld::OutputEHdr::OutputEHdr()
{
    Name = "ELF File Header";
    Shdr.sh_addralign = 8;
    Shdr.sh_size = ELF::EHDR64_SIZE;
    Shdr.sh_flags = SHF_ALLOC;
}

void rvld::OutputEHdr::CopyBuf(Context& ctx)
{

    auto eHdr = ELF::Elf64_Ehdr{};
    eHdr.e_ident[EI_MAG0] = ELFMAG0;
    eHdr.e_ident[EI_MAG1] = ELFMAG1;
    eHdr.e_ident[EI_MAG2] = ELFMAG2;
    eHdr.e_ident[EI_MAG3] = ELFMAG3;
    eHdr.e_ident[EI_CLASS] = ELFCLASS64;
    eHdr.e_ident[EI_DATA] = ELFDATA2LSB;
    eHdr.e_ident[EI_VERSION] = EV_CURRENT;
    eHdr.e_ident[EI_OSABI] = 0;
    eHdr.e_ident[EI_ABIVERSION] = 0;

    eHdr.e_type = ET_EXEC;
    eHdr.e_machine = EM_RISCV;
    eHdr.e_version = EV_CURRENT;
    // TODO
    eHdr.e_ehsize = ELF::EHDR64_SIZE;
    eHdr.e_phentsize = ELF::PHDR64_SIZE;
    // TODO
    eHdr.e_shentsize = ELF::SHDR64_SIZE;

    auto bytes = Bytes{};
    bytes.Write<ELF::Elf64_Ehdr>(eHdr);
    ctx.BufferRef()->Write(GetSHdr()->sh_offset, bytes);
}

ChunkKind rvld::OutputEHdr::GetKind()
{
    return ChunkKind::HEADER;
}