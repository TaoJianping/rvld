//
// Created by tjp56 on 2023/6/28.
//

#include "rvld.h"
#include "utils/Utils.h"

rvld::MergedSection::MergedSection(std::string name, uint64_t flags, uint32_t type)
{
    Name = std::move(name);
    Shdr.sh_flags = flags;
    Shdr.sh_type = type;
}

rvld::SectionFragment* rvld::MergedSection::Insert(const Bytes& key, uint32_t p2align)
{
    SectionFragment* sf;

    if (_sectionFragments.count(key))
    {
        sf = _sectionFragments.at(key);
    }
    else
    {
        sf = new SectionFragment{};
        sf->OutputSection = this;
        _sectionFragments[key] = sf;
    }

    if (sf->P2Align < p2align)
    {
        sf->P2Align = p2align;
    }

    return sf;
}

void rvld::MergedSection::AssignOffsets()
{
    std::vector<std::pair<Bytes, SectionFragment*>> fragments;

    using ft = std::vector<std::pair<Bytes, SectionFragment*>>::value_type;

    for (auto [key, val] : _sectionFragments)
    {
        fragments.emplace_back(key, val);
    }

    std::sort(fragments.begin(), fragments.end(), [](ft& x, ft& y) {
        if (x.second->P2Align != y.second->P2Align) {
            return x.second->P2Align < y.second->P2Align;
        }
        if (x.first.size() != y.first.size()) {
            return x.first.size() < y.first.size();
        }

        return x.first.ToString() < y.first.ToString();
    });

    uint64_t offset = 0;
    uint64_t p2Align = 0;

    for (auto & frag : fragments)
    {
        offset = Utils::AlignTo(offset, frag.second->P2Align);
        frag.second->Offset = offset;
        offset += frag.first.size();
        if (p2Align < frag.second->P2Align) {
            p2Align = frag.second->P2Align;
        }
    }

    auto shdr = GetSHdr();
    shdr->sh_size = offset;
    shdr->sh_addralign = p2Align;
}

void rvld::MergedSection::CopyBuf(Context& ctx)
{
    auto base = GetSHdr()->sh_offset;
    auto buffer = ctx.BufferRef();
    for (const auto& [key, val] : _sectionFragments)
    {
        buffer->Write(base + val->Offset, key);
    }
}
