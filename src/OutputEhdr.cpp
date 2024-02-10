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
    auto GetProgramEntry = [](Context& ctx) -> uint64_t{
        for (auto osec : ctx.OutputSections())
        {
            if (osec->Name == ".text") {
                return osec->Shdr.sh_addr;
            }
        }

        assert(false);
    };

    auto getFlags = [](Context& ctx) -> uint64_t{
        assert(!ctx.GetObjectFiles().empty());

        auto objects = ctx.GetObjectFiles();
        auto flags = objects.at(0)->GetSourceFile()->ElfHeader().e_flags;
        for (size_t i = 1; i < objects.size(); i++)
        {
            auto ehdr = objects.at(i)->GetSourceFile()->ElfHeader();
            if ((ehdr.e_flags & EF_RISCV_RVC) != 0) {
                flags |= EF_RISCV_RVC;
            }
        }
        return flags;
    };


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
    // program entry
    eHdr.e_entry = GetProgramEntry(ctx);
    eHdr.e_phoff = ctx.ProgramHeaderTable()->GetSHdr()->sh_offset;
    eHdr.e_phentsize = ELF::PHDR64_SIZE;
    eHdr.e_phnum = ctx.ProgramHeaderTable()->GetSHdr()->sh_size / ELF::PHDR64_SIZE;
    // section header
    eHdr.e_shoff = ctx.SectionHeaderTable()->GetSHdr()->sh_offset;
    eHdr.e_shnum = ctx.SectionHeaderTable()->GetSHdr()->sh_size / ELF::SHDR64_SIZE;
    eHdr.e_shentsize = ELF::SHDR64_SIZE;

    eHdr.e_ehsize = ELF::EHDR64_SIZE;
    // TODO

    eHdr.e_flags = getFlags(ctx);

    auto bytes = Bytes{};
    bytes.Write<ELF::Elf64_Ehdr>(eHdr);
    ctx.BufferRef()->Write(GetSHdr()->sh_offset, bytes);
}

ChunkKind rvld::OutputEHdr::GetKind()
{
    return ChunkKind::HEADER;
}