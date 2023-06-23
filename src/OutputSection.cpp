//
// Created by tjp56 on 2023/6/20.
//
#include "rvld.h"


rvld::OutputSection::OutputSection(std::string name, uint32_t type, uint64_t flags)
{
    Name = std::move(name);
    Shdr.sh_type = type;
    Shdr.sh_flags = flags;
}

void rvld::OutputSection::AppendInputSection(rvld::InputSection* isec)
{
    _members.push_back(isec);
}

size_t rvld::OutputSection::MembersCount()
{
    return _members.size();
}

std::vector<rvld::InputSection*> rvld::OutputSection::ReadMembers()
{
    return _members;
}

ChunkKind rvld::OutputSection::GetKind()
{
    return ChunkKind::OUTPUT_SECTION;
}

void rvld::OutputSection::UpdateShdr(Context& ctx)
{

}

void rvld::OutputSection::CopyBuf(Context& ctx)
{
    if (Shdr.sh_type == SHT_NOBITS) {
        return ;
    }

    auto buffer = ctx.BufferRef();
    auto base = Shdr.sh_offset;
    assert(buffer != nullptr);

    for (auto isec : ReadMembers())
    {
        auto sh = isec->GetSectionHeader();
        if (sh.sh_type == SHT_NOBITS || isec->ShSize() == 0) {
            continue ;
        }

        buffer->Write(base + isec->GetOffset(), isec->Content());
    }
}