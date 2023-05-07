#include <iostream>
#include "ELFFile.h"

ELF::ElfFile::ElfFile(InputFile* file) :
    _localFile(file)
{
    _initialize();
}

InputFile* ELF::ElfFile::GetSourceFile()
{
    return _localFile;
}

ELF::Elf64_Ehdr ELF::ElfFile::GetELFHeader()
{
    return _elfHeader;
}

std::vector<ELF::Elf64_Shdr> ELF::ElfFile::GetSectionHeaders()
{
    return _elfSectionHeaders;
}

ELF::Elf64_Ehdr ELF::ElfFile::ReadELFHeader()
{
    auto ehdr = ELF::Elf64_Ehdr{};
    std::memcpy(&ehdr, GetSourceFile()->GetContentView().data(), EHDR64_SIZE);
    return ehdr;
}

std::vector<ELF::Elf64_Shdr> ELF::ElfFile::ReadSectionHeaders()
{
    std::vector<ELF::Elf64_Shdr> ret{};
    auto sourceFile = GetSourceFile();
    auto ehdr = GetELFHeader();
    auto fileContents = sourceFile->GetContentView();
    auto sectionNumber = ehdr.e_shnum;
    auto sectionOffset = ehdr.e_shoff;
    auto start = sectionOffset;

    for (uint16_t i = 0; i < sectionNumber; i++)
    {
        Elf64_Shdr sh{};
        std::memcpy(&sh, fileContents.data() + start, SHDR64_SIZE);
        start = start + SHDR64_SIZE;
        ret.emplace_back(sh);
    }

    return ret;
}

ELF::Elf64_Shdr ELF::ElfFile::ReadSectionHeader(uint16_t index)
{
    return _elfSectionHeaders.at(index);
}

BytesView ELF::ElfFile::ReadSectionContent(ELF::Elf64_Shdr shdr)
{
    auto start = shdr.sh_offset;
    auto size = shdr.sh_size;
    return ReadContentView(start, size);
}

BytesView ELF::ElfFile::ReadSectionContent(uint16_t index)
{
    auto sh = ReadSectionHeader(index);
    auto start = sh.sh_offset;
    auto size = sh.sh_size;
    return ReadContentView(start, size);
}

ELF::Elf64_Shdr ELF::ElfFile::ReadSectionNameStringTableHeader()
{
    auto eh = GetELFHeader();
    return ReadSectionHeader(eh.e_shstrndx);
}

gsl::span<std::byte> ELF::ElfFile::ReadSectionNameStringTable()
{
    auto eh = ReadELFHeader();
    return ReadSectionContent(eh.e_shstrndx);
}

std::string ELF::ElfFile::ReadSectionName(ELF::Elf64_Shdr shdr)
{
    auto start = _sectionNameStringTable.begin() + shdr.sh_name;
    auto name = std::string{};
    for (;*start != std::byte{0x00}; start++)
    {
        name.push_back(static_cast<char>(*start));
    }
    return name;
}

std::optional<ELF::Elf64_Shdr> ELF::ElfFile::FindSectionHeader(ELF::ElfSectionType ty)
{
    auto sectionHeaders = GetSectionHeaders();
    for (auto sh : sectionHeaders)
    {
        if (sh.sh_type == static_cast<uint32_t>(ty)) {
            return sh;
        }
    }

    return std::nullopt;
}

Bytes ELF::ElfFile::ReadRowBytesContent(uint64_t start, uint64_t size)
{
    auto content = GetSourceFile()->GetContentView();
    Bytes data{};
    auto first = content.begin() + static_cast<long>(start);
    auto last = content.begin() + static_cast<long>(start) + static_cast<long>(size);
    data.insert(data.begin(), first, last);
    return data;
}

ELF::Elf64_Shdr ELF::ElfFile::ReadSymbolTableSectionHeader()
{
    auto sh = FindSectionHeader(ElfSectionType::SHT_SYMTAB);
    if (!sh.has_value())
    {
        LOG(ERROR) << "Not Find System Table Section Header.";
    }
    return sh.value();
}

std::vector<ELF::Elf64_Sym> ELF::ElfFile::ReadSymbolTable()
{
    auto symShdr = ReadSymbolTableSectionHeader();
    auto content = ReadSectionContent(symShdr);
    auto num = content.size() / ELF::Sym64_SIZE;
    auto res = ReadStructVector<Elf64_Sym>(symShdr.sh_offset, num);
    return res;
}

ELF::Elf64_Shdr ELF::ElfFile::ReadStringTableSectionHeader()
{
    auto symShdr = ReadSymbolTableSectionHeader();
    auto stringTableSectionHeader = ReadSectionHeader(symShdr.sh_link);
    return stringTableSectionHeader;
}

BytesView ELF::ElfFile::ReadStringTable()
{
    auto symShdr = ReadSymbolTableSectionHeader();
    auto stringTableSection = ReadSectionHeader(symShdr.sh_link);
    auto stringTable = ReadSectionContent(stringTableSection);
    return stringTable;
}

std::string ELF::ElfFile::ReadSymbolName(ELF::Elf64_Sym sym)
{
    auto stringTable = ReadStringTable();
    auto start = stringTable.begin() + sym.st_name;
    auto name = std::string{};
    for (;*start != std::byte{0x00}; start++)
    {
        name.push_back(static_cast<char>(*start));
    }
    return name;
}

std::string ELF::ElfFile::GetName()
{
    return GetSourceFile()->GetName();
}

bool ELF::ElfFile::_initialize()
{
    _elfHeader = ReadELFHeader();
    _elfSectionHeaders = ReadSectionHeaders();
    _sectionNameStringTable = ReadSectionNameStringTable();
    return true;
}

BytesView ELF::ElfFile::ReadContentView(size_t start, size_t size)
{
    auto content = this->GetSourceFile()->GetContentView();
    return BytesView{content.data() + start, size};
}

template<typename T>
std::vector<T> ELF::ElfFile::ReadStructVector(uint64_t start, uint64_t num)
{
    std::vector<T> ret{};
    auto sourceFile = GetSourceFile();
    auto fileContents = sourceFile->GetContentView();
    auto itemSize = sizeof(T{});

    for (uint32_t i = 0; i < num; i++)
    {
        T item{};
        std::memcpy(&item, fileContents.data() + start, itemSize);
        start = start + itemSize;
        ret.emplace_back(item);
    }

    return ret;
}
