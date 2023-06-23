#include <iostream>
#include <utility>
#include <cassert>
#include <cmath>

#include "rvld.h"

rvld::ElfFile::ElfFile(InputFile* file) :
    _localFile(file)
{
    _initialize();
}

InputFile* rvld::ElfFile::GetSourceFile()
{
    assert(_localFile != nullptr);
    return _localFile;
}

ELF::Elf64_Ehdr rvld::ElfFile::ReadELFHeader()
{
    return _elfHeader;
}

std::vector<ELF::Elf64_Shdr> rvld::ElfFile::GetSectionHeaders()
{
    return _elfSectionHeaders;
}

std::vector<ELF::Elf64_Shdr> rvld::ElfFile::ReadSectionHeaders()
{
    std::vector<ELF::Elf64_Shdr> ret{};
    auto sourceFile = GetSourceFile();
    auto ehdr = ReadELFHeader();
    auto fileContents = sourceFile->GetContentView();
    auto sectionNumber = GetSectionHeaderCount();
    auto sectionOffset = ehdr.e_shoff;
    auto start = sectionOffset;

    for (auto i = 0; i < sectionNumber; i++)
    {
        ELF::Elf64_Shdr sh{};
        std::memcpy(&sh, fileContents.data() + start, ELF::SHDR64_SIZE);
        start = start + ELF::SHDR64_SIZE;
        ret.emplace_back(sh);
    }

    return ret;
}

ELF::Elf64_Shdr rvld::ElfFile::GetSectionHeader(uint16_t index)
{
    return _elfSectionHeaders.at(index);
}

BytesView rvld::ElfFile::ReadSectionContent(ELF::Elf64_Shdr shdr)
{
    auto start = shdr.sh_offset;
    auto size = shdr.sh_size;
    return ReadContentView(start, size);
}

BytesView rvld::ElfFile::ReadSectionContent(uint16_t index)
{
    auto sh = GetSectionHeader(index);
    auto start = sh.sh_offset;
    auto size = sh.sh_size;
    return ReadContentView(start, size);
}

size_t rvld::ElfFile::GetSectionNameStringTableIndex()
{
    auto ehdr = ReadELFHeader();
    size_t idx = ehdr.e_shstrndx;
    if (idx == static_cast<size_t>(ELF::ElfSpecialSectionIndex::XINDEX))
    {
        auto shr = GetSectionHeader(0);
        idx = shr.sh_link;
    }
    return idx;
}

ELF::Elf64_Shdr rvld::ElfFile::ReadSectionNameStringTableHeader()
{
    auto idx = GetSectionNameStringTableIndex();
    return GetSectionHeader(idx);
}

BytesView rvld::ElfFile::ReadSectionNameStringTable()
{
    auto idx = GetSectionNameStringTableIndex();
    return ReadSectionContent(idx);
}

std::string rvld::ElfFile::ReadSectionName(ELF::Elf64_Shdr shdr)
{
    auto start = _sectionNameStringTable.begin() + shdr.sh_name;
    auto name = std::string{};
    for (; *start != std::byte{ 0x00 }; start++)
    {
        name.push_back(static_cast<char>(*start));
    }
    return name;
}

std::optional<ELF::Elf64_Shdr> rvld::ElfFile::FindSectionHeader(ELF::ElfSectionType ty)
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

Bytes rvld::ElfFile::ReadRowBytesContent(uint64_t start, uint64_t size)
{
    auto content = GetSourceFile()->GetContentView();
    Bytes data{};
    auto first = content.begin() + static_cast<long>(start);
    auto last = content.begin() + static_cast<long>(start) + static_cast<long>(size);
    data.insert(data.begin(), first, last);
    return data;
}

ELF::Elf64_Shdr rvld::ElfFile::ReadSymbolTableSectionHeader()
{
    auto sh = FindSectionHeader(ELF::ElfSectionType::SYMTAB);
    if (!sh.has_value())
    {
        spdlog::error("Not Find Symbol Table Section Header. File name is {}", GetName());
    }
    return sh.value();
}

std::vector<ELF::Elf64_Sym> rvld::ElfFile::ReadSymbolTable()
{
    if (!HasSymbolSection())
    {
        //        spdlog::info("This ELF File has no symbol section");
        return {};
    }
    auto symShdr = ReadSymbolTableSectionHeader();
    auto content = ReadSectionContent(symShdr);
    auto num = content.size() / ELF::Sym64_SIZE;
    std::vector<ELF::Elf64_Sym> res = ReadStructVector<ELF::Elf64_Sym>(symShdr.sh_offset, num);
    return res;
}

ELF::Elf64_Shdr rvld::ElfFile::ReadStringTableSectionHeader()
{
    auto symShdr = ReadSectionNameStringTableHeader();
    auto stringTableSectionHeader = GetSectionHeader(symShdr.sh_link);
    return stringTableSectionHeader;
}

BytesView rvld::ElfFile::ReadSymbolTableStringTable()
{
    auto symShdr = ReadSymbolTableSectionHeader();
    auto stringTableSection = GetSectionHeader(symShdr.sh_link);
    auto stringTable = ReadSectionContent(stringTableSection);
    return stringTable;
}

std::string rvld::ElfFile::ReadSymbolName(ELF::Elf64_Sym sym)
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

std::string rvld::ElfFile::GetName()
{
    return GetSourceFile()->GetName();
}

bool rvld::ElfFile::_initialize()
{
    // Initialize elf header
    std::memcpy(&_elfHeader, GetSourceFile()->GetContentView().data(), ELF::EHDR64_SIZE);

    // Initialize elf section header table
    auto sourceFile = GetSourceFile();
    auto fileContents = sourceFile->GetContentView();
    auto sectionNumber = GetSectionHeaderCount();
    auto start = _elfHeader.e_shoff;

    for (auto i = 0; i < sectionNumber; i++)
    {
        ELF::Elf64_Shdr sh{};
        std::memcpy(&sh, fileContents.data() + start, ELF::SHDR64_SIZE);
        start = start + ELF::SHDR64_SIZE;
        _elfSectionHeaders.emplace_back(sh);
    }

    _sectionNameStringTable = ReadSectionNameStringTable();

    return true;
}

BytesView rvld::ElfFile::ReadContentView(size_t start, size_t size)
{
    auto content = this->GetSourceFile()->GetContentView();
    return BytesView{ content.data() + start, size };
}

size_t rvld::ElfFile::GetSectionHeaderCount()
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

MachineType rvld::ElfFile::GetMachineType()
{
    auto ehdr = ReadELFHeader();
    if (ehdr.e_machine == static_cast<uint16_t>(ELF::ELFMachine::RISC_V))
    {
        return MachineType::RISCV64;
    }
    return MachineType::None;
}

bool rvld::ElfFile::HasSymbolSection()
{
    return FindSectionHeader(ELF::ElfSectionType::SYMTAB).has_value();
}

template<typename T>
std::vector<T> rvld::ElfFile::ReadStructVector(uint64_t start, uint64_t num)
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

rvld::ObjectFile::ObjectFile(InputFile* localFile) :
    ElfFile(localFile)
{
    _InitializeInputSections();
    _InitializeSymbols();
    _InitializeMergeableSections();
}

void rvld::ObjectFile::_InitializeInputSections()
{
    auto shCount = GetSectionHeaderCount();
    if (shCount == 0)
        return;

//    spdlog::info("File {} has {} sections", GetName(), shCount);
    _inputSections.insert(_inputSections.end(), shCount, nullptr);

    for (size_t i = 0; i < shCount; i++)
    {
        auto sh = GetSectionHeader(i);
        switch (static_cast<ELF::ElfSectionType>(sh.sh_type))
        {
        case ELF::ElfSectionType::GROUP:
        case ELF::ElfSectionType::STRTAB:
        case ELF::ElfSectionType::REL:
        case ELF::ElfSectionType::RELA:
        case ELF::ElfSectionType::NULL_:
        {
            break ;
        }
        case ELF::ElfSectionType::SYMTAB:
        {
            // spdlog::info("One greater than the symbol table index of the last local symbol (binding STB_LOCAL) -> {}", sh.sh_info);
            _globalSymbolStartIndex = sh.sh_info;
            break ;
        }
        case ELF::ElfSectionType::SYMTAB_SHNDX:
        {
            // initialize SHT_SYMTAB_SHNDX
            // If symbol st_shndx contains SHN_XINDEX, then the actual section header index is too large to fit in this field.
            // The actual value is contained in the associated section of type SHT_SYMTAB_SHNDX.
            _symbolTableShndxSectionIdx = static_cast<int64_t>(i);
        }
        default:
        {
            auto is = new InputSection{ this, static_cast<uint32_t>(i) };
            _inputSections[i] = is;
        }
        }

    }
}

void rvld::ObjectFile::_InitializeSymbols()
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

int64_t rvld::ObjectFile::GetGlobalSymbolStartIndex()
{
    return _globalSymbolStartIndex;
}

rvld::InputSection* rvld::ObjectFile::_GetInputSection(ELF::Elf64_Sym symbol, size_t index)
{
    return GetInputSections().at(GetShnIndex(symbol, index));
}

std::vector<rvld::InputSection*> rvld::ObjectFile::GetInputSections()
{
    return _inputSections;
}

std::vector<rvld::Symbol*> rvld::ObjectFile::Symbols()
{
    return _symbols;
}

std::vector<rvld::Symbol*> rvld::ObjectFile::GetGlobalSymbols()
{
    std::vector<Symbol*> ret{};

    if (!HasGlobalSymbol())
    {
        return ret;
    }

    auto symbols = Symbols();

    assert(GetGlobalSymbolStartIndex() < static_cast<int64_t>(symbols.size()));

    for (size_t i = GetGlobalSymbolStartIndex(); i < symbols.size(); ++i)
    {
        ret.push_back(symbols[i]);
    }

    return ret;
}

bool rvld::ObjectFile::HasGlobalSymbol()
{
    auto index = GetGlobalSymbolStartIndex();
    return (index != -1) && (index < Symbols().size());
}

void rvld::ObjectFile::_InitializeMergeableSections()
{
    auto inputSections = GetInputSections();
    _mergeableSections.resize(inputSections.size(), nullptr);

    for (size_t i = 0; i < inputSections.size(); ++i)
    {
        auto isec = inputSections[i];
//        if (isec != nullptr)
//        {
////            spdlog::info("Handle mergeable section -> {}", isec->SectionName());
//            if (isec->SectionName() == ".srodata.cst4")
//            {
//                if (isec->IsMergeableSection()) {
//                    spdlog::info(".srodata.cst4 section is mergeable");
//                }
//                if (isec->IsAlive())
//                {
//                    spdlog::info(".srodata.cst4 section is Alive");
//                }
//            }
//        }
        if (isec != nullptr && isec->IsMergeableSection())
        {
//            spdlog::info("Handle mergeable section -> {}", isec->SectionName());
            _mergeableSections[i] = _SplitSection(isec);
            isec->SetAliveStatus(false);
        }
    }
}

size_t FindNull(Bytes& data, size_t entSize)
{
    Bytes zero{};
    zero.resize(entSize, std::byte{ 0x00 });

    if (entSize == 1)
    {
        auto it = std::find(data.begin(), data.end(), std::byte{ '\0' });
        return std::distance(data.begin(), it);
    }

    for (size_t i = 0; i <= data.size() - entSize; i += entSize)
    {
        auto subData = data.SubBytes(i, i + entSize);
        if (std::all_of(subData.begin(), subData.end(), [](std::byte item) { return item == std::byte{ 0x00 }; }))
        {
            return i;
        }
    }

    return -1;
};

rvld::MergeableSection* rvld::ObjectFile::_SplitSection(InputSection* isec)
{
    auto ms = new MergeableSection{};
    auto sHdr = isec->GetSectionHeader();

    ms->P2Align = isec->P2Align();
    ms->Name = isec->SectionName();
    ms->Shdr = sHdr;

    auto data = isec->Content();
    assert(!data.empty());
    uint64_t offset = 0;
    if ((sHdr.sh_flags & SHF_STRINGS) != 0)
    {
        while (offset < data.size())
        {
            auto end = FindNull(data, sHdr.sh_entsize);
            if (end == -1)
            {
                spdlog::error("-1 -> file name is {}, {}", this->GetSourceFile()->GetName(), isec->SectionName());
            }

            auto subData = data.SubBytes(0, end + sHdr.sh_entsize);
            data = data.SubBytes(end + sHdr.sh_entsize);
            ms->AppendData(subData);
            ms->AppendOffset(offset);
            offset += (end + sHdr.sh_entsize);
        }
//        spdlog::info("This file name is -> {}", GetSourceFile()->GetName());
//        spdlog::info("SHF_STRINGS Generate Offsets -> {}", fmt::join(ms->GetOffsets(), ","));
    }
    else
    {
        auto entrySize = sHdr.sh_entsize;
        assert((data.size() % sHdr.sh_entsize) == 0);
        auto entryCount = data.size() / sHdr.sh_entsize;
        for (auto i = 0; i < entryCount; i++)
        {
            auto subData = data.SubBytes(offset, offset + entrySize);
            ms->AppendData(subData);
            ms->AppendOffset(offset);
            offset += entrySize;
        }
//        spdlog::info("This file name is -> {}", GetSourceFile()->GetName());
//        spdlog::info("Common Generate Offsets -> {}", fmt::join(ms->GetOffsets(), ","));
    }


    return ms;
}

std::vector<rvld::MergeableSection*> rvld::ObjectFile::GetMergeableSections()
{
    return _mergeableSections;
}

void rvld::ObjectFile::RegisterSectionPieces()
{
    for (auto&& ms : GetMergeableSections())
    {
        if (ms == nullptr) {
            continue ;
        }

        auto data = ms->GetData();
        auto offsets = ms->GetOffsets();

        assert(data.size() == offsets.size());

        for (auto i = 0; i < data.size(); ++i)
        {
            auto byte = data[i];
            auto offset = offsets[i];

            ms->AppendFragments(ms->Parent->Insert(byte, ms->P2Align));
        }
    }

    auto symbols = Symbols();
    auto mergeableSections = GetMergeableSections();

    for (auto i = 0; i < symbols.size(); ++i)
    {
        auto sym = symbols[i];
        auto eSym = sym->ELFSymbol();

        if (eSym.IsAbsSymbol() || eSym.IsUndefinedSymbol() || eSym.IsCommonSymbol())
        {
            continue;
        }

        auto ms = mergeableSections[GetShnIndex(eSym, i)];
        if (ms == nullptr)
        {
            continue;
        }

        auto [frag, offset] = ms->GetSectionFragment(eSym.st_value);

        assert(frag != nullptr);
//
        sym->SetSectionFragment(frag);
        sym->SetValue(offset);
    }
}

uint32_t rvld::ObjectFile::GetShnIndex(ELF::Elf64_Sym elf64Sym, size_t index)
{
    if (elf64Sym.NeedExtendSection())
    {
        spdlog::info("Need Extend Section");
        // Todo 可以優化
        auto symbolTableShnSection = ReadSectionContent(_symbolTableShndxSectionIdx);
        auto idx = Bytes{symbolTableShnSection}.ReadStruct<uint32_t>(index * 4);
        return idx;
    }

    return elf64Sym.st_shndx;
}

void rvld::ObjectFile::AppendSymbol(rvld::Symbol* sym)
{
    _symbols.push_back(sym);
}

void rvld::ObjectFile::SetGlobalSymbolIndex(int64_t idx)
{
    _globalSymbolStartIndex = idx;
}
