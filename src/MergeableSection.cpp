//
// Created by tjp56 on 2023/6/5.
//

#include <utility>

#include "rvld.h"



void rvld::MergeableSection::AppendOffset(uint64_t offset)
{
    _offsets.push_back(offset);
}

void rvld::MergeableSection::AppendData(Bytes data)
{
    _data.push_back(data);
}

void rvld::MergeableSection::AppendFragments(rvld::SectionFragment* sf)
{
    _fragments.push_back(sf);
}

std::vector<Bytes> rvld::MergeableSection::GetData()
{
    return _data;
}

std::vector<uint64_t> rvld::MergeableSection::GetOffsets()
{
    return _offsets;
}

std::pair<rvld::SectionFragment*, uint64_t> rvld::MergeableSection::GetSectionFragment(uint64_t offset)
{
//    assert(GetOffsets().size() == GetData().size() && GetData().size() == _fragments.size());

    const auto& offsets = _offsets;
    auto it = std::upper_bound(offsets.begin(), offsets.end(), offset);
//    auto it = std::find(offsets.begin(), offsets.end(), offset);

    if (it == offsets.end())
    {
        //        spdlog::error("offset is -> {}", offset);
        //        spdlog::error("offsets is -> {}", fmt::join(offsets, ","));
    }

    auto index = it - 1 - offsets.begin();
    auto fragment = _fragments.at(index);

    return std::make_pair(fragment, offset - offsets.at(index));
}




