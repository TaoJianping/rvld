#pragma once

#include "InputFile.h"
#include "Define.h"
#include "Utils/Bytes.h"
#include <optional>


namespace ELF
{
    class ElfFile
    {
    public:
        explicit ElfFile(InputFile* file);

        Elf64_Ehdr GetELFHeader();
        Elf64_Ehdr ReadELFHeader();
        std::vector<Elf64_Shdr> GetSectionHeaders();
        std::vector<Elf64_Shdr> ReadSectionHeaders();
        Elf64_Shdr ReadSectionNameStringTableHeader();
        BytesView ReadSectionNameStringTable();
        std::string ReadSectionName(Elf64_Shdr shdr);
        Elf64_Shdr ReadSymbolTableSectionHeader();
        std::vector<Elf64_Sym> ReadSymbolTable();
        Elf64_Shdr ReadStringTableSectionHeader();
        BytesView ReadStringTable();
        std::string ReadSymbolName(Elf64_Sym sym);
        std::optional<ELF::Elf64_Shdr> FindSectionHeader(ElfSectionType ty);
        InputFile* GetSourceFile();
        std::string GetName();

        template<typename T>
        std::vector<T> ReadStructVector(uint64_t start, uint64_t num);

        BytesView ReadSectionContent(Elf64_Shdr shdr);
        BytesView ReadSectionContent(uint16_t index);
        Elf64_Shdr ReadSectionHeader(uint16_t index);
        BytesView ReadContentView(size_t start, size_t size);
        Bytes ReadRowBytesContent(size_t start, size_t size);

    private:
        InputFile* _localFile = nullptr;
        Elf64_Ehdr _elfHeader;
        std::vector<Elf64_Shdr> _elfSectionHeaders;
        std::vector<Elf64_Sym> _elfSymbols;
        BytesView _sectionNameStringTable;

        bool _initialize();


    };
}
