#include <iostream>
#include <utility>
#include <cassert>
#include "ELFFile.h"

ELF::ElfFile::ElfFile(InputFile* file) :
    _localFile(file)
{
    _initialize();
}

InputFile* ELF::ElfFile::GetSourceFile()
{
    assert(_localFile != nullptr);
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
    auto sectionNumber = GetSectionHeaderCount();
    auto sectionOffset = ehdr.e_shoff;
    auto start = sectionOffset;

    for (auto i = 0; i < sectionNumber; i++)
    {
        Elf64_Shdr sh{};
        std::memcpy(&sh, fileContents.data() + start, SHDR64_SIZE);
        start = start + SHDR64_SIZE;
        ret.emplace_back(sh);
    }

    return ret;
}

ELF::Elf64_Shdr ELF::ElfFile::GetSectionHeader(uint16_t index)
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
    auto sh = GetSectionHeader(index);
    auto start = sh.sh_offset;
    auto size = sh.sh_size;
    return ReadContentView(start, size);
}

size_t ELF::ElfFile::GetSectionNameStringTableIndex()
{
    auto ehdr = GetELFHeader();
    size_t idx = ehdr.e_shstrndx;
    if (idx == static_cast<size_t>(ELF::ElfSpecialSectionIndex::XINDEX))
    {
        auto shr = GetSectionHeader(0);
        idx = shr.sh_link;
    }
    return idx;
}

ELF::Elf64_Shdr ELF::ElfFile::ReadSectionNameStringTableHeader()
{
    auto idx = GetSectionNameStringTableIndex();
    return GetSectionHeader(idx);
}

BytesView ELF::ElfFile::ReadSectionNameStringTable()
{
    auto idx = GetSectionNameStringTableIndex();
    return ReadSectionContent(idx);
}

std::string ELF::ElfFile::ReadSectionName(ELF::Elf64_Shdr shdr)
{
    auto start = _sectionNameStringTable.begin() + shdr.sh_name;
    auto name = std::string{};
    for (; *start != std::byte{ 0x00 }; start++)
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
        if (sh.sh_type == static_cast<uint32_t>(ty))
        {
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
        spdlog::error("Not Find Symbol Table Section Header. File name is {}", GetName());
    }
    return sh.value();
}

std::vector<ELF::Elf64_Sym> ELF::ElfFile::ReadSymbolTable()
{
    if (!HasSymbolSection())
    {
//        spdlog::info("This ELF File has no symbol section");
        return {};
    }
    auto symShdr = ReadSymbolTableSectionHeader();
    auto content = ReadSectionContent(symShdr);
    auto num = content.size() / ELF::Sym64_SIZE;
    auto res = ReadStructVector<Elf64_Sym>(symShdr.sh_offset, num);
    return res;
}

ELF::Elf64_Shdr ELF::ElfFile::ReadStringTableSectionHeader()
{
    auto symShdr = ReadSectionNameStringTableHeader();
    auto stringTableSectionHeader = GetSectionHeader(symShdr.sh_link);
    return stringTableSectionHeader;
}

BytesView ELF::ElfFile::ReadSymbolTableStringTable()
{
    auto symShdr = ReadSymbolTableSectionHeader();
    auto stringTableSection = GetSectionHeader(symShdr.sh_link);
    auto stringTable = ReadSectionContent(stringTableSection);
    return stringTable;
}

std::string ELF::ElfFile::ReadSymbolName(ELF::Elf64_Sym sym)
{
//    if (!HasSymbolSection())
//    {
//        spdlog::info("This ELF File has no symbol section");
//        return "";
//    }
    auto stringTable = ReadSymbolTableStringTable();
    auto start = stringTable.begin() + sym.st_name;
    auto name = std::string{};
    for (; *start != std::byte{ 0x00 }; start++)
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

//    if (HasSymbolSection())
//    {
//        _elfSymbols = ReadSymbolTable();
//    }
    return true;
}

BytesView ELF::ElfFile::ReadContentView(size_t start, size_t size)
{
    auto content = this->GetSourceFile()->GetContentView();
    return BytesView{ content.data() + start, size };
}

size_t ELF::ElfFile::GetSectionHeaderCount()
{
    /*
     * If the number of sections is greater than or equal to SHN_LORESERVE (0xff00), e_shnum has the value SHN_UNDEF (0)
     * and the actual number of section header table entries is contained in the sh_size field of the section header at index 0 (otherwise,
     * the sh_size member of the initial entry contains 0).
    * */
    auto eh = ReadELFHeader();
    size_t numSection = 0;
    if (eh.e_shnum == static_cast<uint16_t>(ELF::ElfSpecialSectionIndex::UNDEFINED))
    {
        auto shdr = GetSectionHeader(0);
        numSection = shdr.sh_size;
    }
    else
    {
        numSection = static_cast<size_t>(eh.e_shnum);
    }
    return numSection;
}

MachineType ELF::ElfFile::GetMachineType()
{
    auto ehdr = GetELFHeader();
    if (ehdr.e_machine == static_cast<uint16_t>(ELF::ELFMachine::RISC_V))
    {
        return MachineType::RISCV64;
    }
    return MachineType::None;
}

bool ELF::ElfFile::HasSymbolSection()
{
    return FindSectionHeader(ElfSectionType::SHT_SYMTAB).has_value();
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

ELF::ObjectFile::ObjectFile(InputFile* localFile) :
    ElfFile(localFile)
{
    _InitializeInputSections();
    _InitializeSymbols();
}

void ELF::ObjectFile::_InitializeInputSections()
{
    auto shCount = GetSectionHeaderCount();
    if (shCount == 0)
        return;

    _inputSections.insert(_inputSections.end(), shCount, nullptr);

    for (size_t i = 0; i < shCount; i++)
    {
        auto sh = GetSectionHeader(i);
        switch (static_cast<ElfSectionType>(sh.sh_type))
        {
            //        case ElfSectionType::SHT_GROUP:
            //        {
            //            break;
            //        }
        case ElfSectionType::SHT_SYMTAB:
        {
//            spdlog::info("One greater than the symbol table index of the last local symbol (binding STB_LOCAL) -> {}", sh.sh_info);
            _globalSymbolStartIndex = sh.sh_info;
            break;
        }
            //        case ElfSectionType::SHT_STRTAB:
            //        {
            //            break;
            //        }
            //        case ElfSectionType::SHT_REL:
            //        {
            //            break;
            //        }
            //        case ElfSectionType::SHT_RELA:
            //        {
            //            break;
            //        }
            //        case ElfSectionType::SHT_NULL:
            //        {
            //            break;
            //        }
        case ElfSectionType::SHT_SYMTAB_SHNDX:
        {
            // initialize SHT_SYMTAB_SHNDX
            auto content = ReadSectionContent(sh);
            _symbolTableShnDx.insert(_symbolTableShnDx.begin(), content.begin(), content.end());
            break;
        }
        default:
        {
        }
        }
        auto is = new InputSection{ this, static_cast<uint32_t>(i) };
        _inputSections[i] = is;
    }
}

void ELF::ObjectFile::_InitializeSymbols()
{
    auto ElfSymbols = ReadSymbolTable();
    if (ElfSymbols.empty())
        return;

    for (auto i = 0; i < ElfSymbols.size(); i++)
    {
        auto eSym = ElfSymbols.at(i);
        auto symbolName = ReadSymbolName(eSym);
        auto symbol = new Symbol(symbolName, this, i);

        if (!eSym.IsAbsSymbol() && !eSym.IsUndefinedSymbol())
        {
            auto section = _GetInputSection(eSym, i);
            symbol->SetInputSection(section);
        }
        _symbols.push_back(symbol);
    }
}

int64_t ELF::ObjectFile::GetGlobalSymbolStartIndex()
{
    return _globalSymbolStartIndex;
}

ELF::InputSection* ELF::ObjectFile::_GetInputSection(ELF::Elf64_Sym symbol, size_t index)
{
    if (symbol.NeedExtendSection())
    {
        spdlog::info("Need Extend Section");
        auto idx = _symbolTableShnDx.ReadStruct<uint32_t>(index * 4);
        return GetInputSections().at(idx);
    }

    return GetInputSections().at(symbol.st_shndx);
}

std::vector<ELF::InputSection*> ELF::ObjectFile::GetInputSections()
{
    return _inputSections;
}

ELF::Symbol* ELF::ObjectFile::_MakeNewSymbol(ELF::Elf64_Sym eSym, size_t index)
{
    auto symbolName = ReadSymbolName(eSym);
    auto symbol = new Symbol(symbolName, this, index);

    if (eSym.IsAbsSymbol() || eSym.IsUndefinedSymbol())
    {
        return symbol;
    }

    auto section = _GetInputSection(eSym, index);
    symbol->SetInputSection(section);
    return symbol;
}

std::vector<ELF::Symbol*> ELF::ObjectFile::Symbols()
{
    return _symbols;
}

std::vector<ELF::Symbol*> ELF::ObjectFile::GetGlobalSymbols()
{
    std::vector<Symbol*> ret{};

    if (!HasGlobalSymbol())
    {
//        spdlog::info("This object has no global symbol -> {}", GetName());
//        spdlog::info("GetGlobal Symbol Index -> {}", GetGlobalSymbolStartIndex());
        return ret;
    }

    auto symbols = Symbols();

//    if (GetGlobalSymbolStartIndex() >= symbols.size()) {
//        spdlog::info("GetGlobalSymbolStartIndex() -> {}", GetGlobalSymbolStartIndex());
//        spdlog::info("symbols.size() -> {}", symbols.size());
//        spdlog::info("file -> {}", GetSourceFile()->GetName());
//        if (GetSourceFile()->ParentFile()) {
//            spdlog::info("file path -> {}", GetSourceFile()->ParentFile()->GetPath().c_str());
//        }
//    }


    assert(GetGlobalSymbolStartIndex() < static_cast<int64_t>(symbols.size()));

    for (size_t i = GetGlobalSymbolStartIndex(); i < symbols.size(); ++i)
    {
        ret.push_back(symbols[i]);
    }

    return ret;
}

bool ELF::ObjectFile::HasGlobalSymbol()
{
    auto index = GetGlobalSymbolStartIndex();
    return (index != -1) && (index < Symbols().size());
}

ELF::InputSection::InputSection(ObjectFile* file, uint32_t ndx) :
    _objectFile(file), _sectionIndex(ndx)
{
    auto contentView = _objectFile->ReadSectionContent(ndx);
    _content.insert(_content.begin(), contentView.begin(), contentView.end());
}

ELF::Elf64_Shdr ELF::InputSection::GetSectionHeader()
{
    if (GetSectionIndex() > GetSourceFile()->GetSectionHeaderCount())
    {
        spdlog::error("Index Overflow");
    }

    return _objectFile->GetSectionHeader(_sectionIndex);
}

ELF::ObjectFile* ELF::InputSection::GetSourceFile()
{
    return _objectFile;
}

size_t ELF::InputSection::GetSectionIndex() const
{
    return _sectionIndex;
}

std::string ELF::InputSection::SectionName()
{
    auto file = GetSourceFile();
    return file->ReadSectionName(GetSectionHeader());
}

ELF::Symbol::Symbol(std::string name, ELF::ObjectFile* source, size_t index) :
    _name(std::move(name)), _sourceFile(source), _symIdx(index)
{
}
void ELF::Symbol::SetInputSection(ELF::InputSection* sec)
{
    _inputSection = sec;
}

ELF::Elf64_Sym ELF::Symbol::ELFSymbol()
{
    auto file = SourceFile();
    auto symbols = file->ReadSymbolTable();
    return symbols.at(_symIdx);
}
ELF::ObjectFile* ELF::Symbol::SourceFile()
{
    return _sourceFile;
}

std::string ELF::Symbol::Name()
{
    return _name;
}

ELF::ObjectFile* ELF::Symbol::DefinitionFile()
{
    return _definitionFile;
}
