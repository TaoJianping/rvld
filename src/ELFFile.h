#pragma once

#include "InputFile.h"
#include "Define.h"
#include "Utils/Bytes.h"
#include <optional>


namespace ELF
{
    class ObjectFile;

    class Section
    {
    };

    class InputSection : public Section
    {
    public:
        InputSection(ObjectFile* file, uint32_t ndx);
        Elf64_Shdr GetSectionHeader();
        ELF::ObjectFile* GetSourceFile();
        [[nodiscard]] size_t GetSectionIndex() const;
        std::string SectionName();

    private:
        ObjectFile* _objectFile;
        uint32_t _sectionIndex;
        Bytes _content;

    };

    class Symbol
    {
    public:
        Symbol(std::string name, ObjectFile* source, size_t symIndex);
        void SetInputSection(InputSection* sec);
        Elf64_Sym ELFSymbol();
        ObjectFile* SourceFile();
        ObjectFile* DefinitionFile();
        std::string Name();

    private:
        ObjectFile* _sourceFile = nullptr;
        ObjectFile* _definitionFile = nullptr;
        InputSection* _inputSection = nullptr;
        std::string _name{};
        uint64_t _value;
        int32_t _symIdx;

    };

    class ElfFile
    {
    public:
        explicit ElfFile(InputFile* file);

        Elf64_Ehdr GetELFHeader();
        Elf64_Ehdr ReadELFHeader();

        std::vector<Elf64_Shdr> GetSectionHeaders();
        std::vector<Elf64_Shdr> ReadSectionHeaders();

        size_t GetSectionNameStringTableIndex();
        Elf64_Shdr ReadSectionNameStringTableHeader();
        BytesView ReadSectionNameStringTable();

        Elf64_Shdr ReadSymbolTableSectionHeader();
        std::vector<Elf64_Sym> ReadSymbolTable();
        std::string ReadSymbolName(Elf64_Sym sym);
        bool HasSymbolSection();

        Elf64_Shdr ReadStringTableSectionHeader();
        BytesView ReadSymbolTableStringTable();

        size_t GetSectionHeaderCount();
        InputFile* GetSourceFile();
        std::string GetName();
        MachineType GetMachineType();

        std::string ReadSectionName(Elf64_Shdr shdr);
        std::optional<ELF::Elf64_Shdr> FindSectionHeader(ElfSectionType ty);

        template<typename T>
        std::vector<T> ReadStructVector(uint64_t start, uint64_t num);

        BytesView ReadSectionContent(Elf64_Shdr shdr);
        BytesView ReadSectionContent(uint16_t index);
        Elf64_Shdr GetSectionHeader(uint16_t index);
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

    class ObjectFile : public ElfFile
    {
    public:
        explicit ObjectFile(InputFile* localFile);
        int64_t GetGlobalSymbolStartIndex();
        std::vector<InputSection*> GetInputSections();
        std::vector<Symbol*> Symbols();
        std::vector<Symbol*> GetGlobalSymbols();
        bool HasGlobalSymbol();

    private:
        ELF::Elf64_Shdr _symbolTableSectionHeader;
        ELF::Elf64_Shdr _stringSectionHeader;
        Bytes _symbolTableShnDx{};
        std::vector<InputSection*> _inputSections;
        std::vector<Symbol*> _symbols;
        int64_t _globalSymbolStartIndex = -1;

        void _InitializeInputSections();
        void _InitializeSymbols();

        Symbol* _MakeNewSymbol(Elf64_Sym elfSym, size_t symbolIndex);
        InputSection* _GetInputSection(Elf64_Sym symbol, size_t index);

    };
}