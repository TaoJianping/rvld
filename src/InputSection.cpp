//
// Created by tjp56 on 2023/6/20.
//
#include "rvld.h"

rvld::InputSection::InputSection(ObjectFile* file, uint32_t ndx) :
    _objectFile(file), _sectionIndex(ndx)
{
    auto contentView = _objectFile->ReadSectionContent(ndx);
    _content.insert(_content.begin(), contentView.begin(), contentView.end());

    auto sHdr = GetSectionHeader();
    assert((sHdr.sh_flags & SHF_COMPRESSED) == 0);
    _shSize = sHdr.sh_size;
}

ELF::Elf64_Shdr rvld::InputSection::GetSectionHeader()
{
    assert(GetSectionIndex() <= GetSourceFile()->GetSectionHeaderCount());
    return _objectFile->GetSectionHeader(_sectionIndex);
}

rvld::ObjectFile* rvld::InputSection::GetSourceFile()
{
    return _objectFile;
}

size_t rvld::InputSection::GetSectionIndex() const
{
    return _sectionIndex;
}

std::string rvld::InputSection::SectionName()
{
    auto file = GetSourceFile();
    return file->ReadSectionName(GetSectionHeader());
}

uint32_t rvld::InputSection::ShSize() const
{
    return _shSize;
}

uint8_t rvld::InputSection::P2Align()
{
    auto sHdr = GetSectionHeader();
    double result = std::log(sHdr.sh_addralign) / std::log(2.0);
    return static_cast<uint8_t>(result);
}

bool rvld::InputSection::IsAlive() const
{
    return _isAlive;
}

bool rvld::InputSection::SetAliveStatus(bool status)
{
    _isAlive = status;
    return _isAlive;
}

bool rvld::InputSection::IsMergeableSection()
{
    return IsAlive() && ((GetSectionHeader().sh_flags & SHF_MERGE) != 0);
}

Bytes rvld::InputSection::Content()
{
    return _content;
}

void rvld::InputSection::LinkOutputSection(rvld::OutputSection* osec)
{
    _outputSection = osec;
}

void rvld::InputSection::SetOffset(uint32_t offset)
{
    _offset = offset;
}

uint32_t rvld::InputSection::GetOffset() {
    return _offset;
};