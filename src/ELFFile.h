#pragma once

#include "InputFile.h"
#include "Define.h"

namespace ELF
{
    class ElfFile
    {
    public:
        explicit ElfFile(InputFile* file);

        Elf64_Ehdr ReadELFHeader();
        Elf64_Ehdr GetELFHeader();
        std::vector<Elf64_Shdr> ReadSectionHeaders();
        std::vector<Elf64_Shdr> GetSectionHeaders();
        Elf64_Shdr ReadSectionNameStringTableHeader();
        BytesVector ReadSectionNameStringTable();
        std::string ReadSectionName(Elf64_Shdr shdr);
        Elf64_Shdr ReadSymbolTableSectionHeader();
        std::vector<Elf64_Sym> ReadSymbolTable();

        Elf64_Shdr ReadStringTableSectionHeader();
        BytesVector ReadStringTable();
        std::string ReadSymbolName(Elf64_Sym sym);

        std::optional<ELF::Elf64_Shdr> FindSectionHeader(ElfSectionType ty);
        InputFile* GetSourceFile();
        BytesVector ReadSectionContent(Elf64_Shdr shdr);
        BytesVector ReadSectionContent(uint16_t index);
        Elf64_Shdr ReadSectionHeader(uint16_t index);
        void PrintRowContent(BytesVector bytesVector);
        void Parse();

        template<typename T>
        std::vector<T> ReadStructVector(uint64_t start, uint64_t num);


    private:
        BytesVector _ReadRowBytesContent(uint64_t start, uint64_t size);

        InputFile* _localFile = nullptr;
        Elf64_Ehdr _elfHeader;
        std::vector<Elf64_Shdr> _elfSectionHeaders;
        std::vector<Elf64_Sym> _elfSymbols;
        BytesVector _sectionNameStringTable;
    };
}
