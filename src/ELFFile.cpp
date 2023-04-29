#include <iostream>
#include "ELFFile.h"

ELF::ElfFile::ElfFile(InputFile* file) :
    _localFile(file)
{
}

InputFile* ELF::ElfFile::GetSourceFile()
{
    return _localFile;
}

ELF::Elf64_Ehdr ELF::ElfFile::ReadELFHeader()
{
    auto ehdr = ELF::Elf64_Ehdr{};
    std::memcpy(&ehdr, GetSourceFile()->GetContents().data(), EHDR64_SIZE);
    return ehdr;
}

ELF::Elf64_Ehdr ELF::ElfFile::GetELFHeader()
{
    return _elfHeader;
}

std::vector<ELF::Elf64_Shdr> ELF::ElfFile::ReadSectionHeaders()
{
    std::vector<ELF::Elf64_Shdr> ret{};
    auto sourceFile = GetSourceFile();
    auto ehdr = GetELFHeader();
    auto fileContents = sourceFile->GetContents();
    auto sectionNumber = ehdr.e_shnum;
    auto sectionOffset = ehdr.e_shoff;
    auto start = sectionOffset;

    for (uint16_t i = 0; i < sectionNumber; i++)
    {
        Elf64_Shdr sh{};
        std::memcpy(&sh, GetSourceFile()->GetContents().data() + start, SHDR64_SIZE);
        start = start + SHDR64_SIZE;
        ret.emplace_back(sh);
    }

    return ret;
}

std::vector<ELF::Elf64_Shdr> ELF::ElfFile::GetSectionHeaders()
{
    return _elfSectionHeaders;
}

ELF::Elf64_Shdr ELF::ElfFile::ReadSectionHeader(uint16_t index)
{
    return _elfSectionHeaders.at(index);
}

BytesVector ELF::ElfFile::ReadSectionContent(ELF::Elf64_Shdr shdr)
{
    auto start = shdr.sh_offset;
    auto size = shdr.sh_size;
    return _ReadRowBytesContent(start, size);
}

BytesVector ELF::ElfFile::ReadSectionContent(uint16_t index)
{
    auto sh = ReadSectionHeader(index);
    auto start = sh.sh_offset;
    auto size = sh.sh_size;
    return _ReadRowBytesContent(start, size);
}

ELF::Elf64_Shdr ELF::ElfFile::ReadSectionNameStringTableHeader()
{
    auto eh = GetELFHeader();
    return ReadSectionHeader(eh.e_shstrndx);
}

BytesVector ELF::ElfFile::ReadSectionNameStringTable()
{
    auto eh = GetELFHeader();
    return ReadSectionContent(eh.e_shstrndx);
}

void ELF::ElfFile::Parse()
{
    // TODO 先把Parse当成所有逻辑的起点把
    _elfHeader = ReadELFHeader();
    _elfSectionHeaders = ReadSectionHeaders();
    _sectionNameStringTable = ReadSectionNameStringTable();
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

BytesVector ELF::ElfFile::_ReadRowBytesContent(uint64_t start, uint64_t size)
{
    auto content = GetSourceFile()->GetContents();

    auto first = content.begin() + static_cast<long>(start);
    auto last = content.begin() + static_cast<long>(start) + static_cast<long>(size);

    return BytesVector {first, last};
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

void ELF::ElfFile::PrintRowContent(BytesVector bytesVector)
{
    std::string str {};
    for (auto v : bytesVector)
    {
        std::cout << static_cast<char>(v);
        str.push_back(static_cast<char>(v));
    }

//    LOG(INFO) << str;
}

ELF::Elf64_Shdr ELF::ElfFile::ReadStringTableSectionHeader()
{
    auto symShdr = ReadSymbolTableSectionHeader();
    auto stringTableSectionHeader = ReadSectionHeader(symShdr.sh_link);
    return stringTableSectionHeader;
}

BytesVector ELF::ElfFile::ReadStringTable()
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

template<typename T>
std::vector<T> ELF::ElfFile::ReadStructVector(uint64_t start, uint64_t num)
{
    std::vector<T> ret{};
    auto sourceFile = GetSourceFile();
    auto fileContents = sourceFile->GetContents();
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
