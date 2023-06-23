#pragma once

#include <cstdint>
#include <string>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <optional>
#include <map>
#include <spdlog/spdlog.h>

#include "Utils/strutil.h"
#include "Utils/InputFile.h"
#include <boost/program_options.hpp>
#include "Define.h"

namespace rvld
{
    class ObjectFile;
    class MergedSection;
    class OutputSection;
    class Context;
    class Symbol;
    class OutputEHdr;
    class OutputSHdr;

    class Linker
    {
    public:
        Linker();

        InputFile* NewInputFile(fs::path path);
        InputFile* NewInputFile(const std::string& path);
        ObjectFile* NewObjectFile(InputFile* localFile);

        Context* NewContext();
        Context* GetContext();
        void SetDefaultContext(Context* ctx);

        bool FillUpObjects(Context* ctx, std::vector<InputFile*> files);
        std::vector<InputFile*> ReadInputFiles(Context* ctx);

        bool CheckFileCompatibility(Context* ctx, InputFile* inputFile);

    private:
        InputFile* FindLibrary(Context* ctx, const std::string& libName);
        Context* _defaultContext = nullptr;
    };

    class Chunk
    {
    public:
        std::string Name{};
        ELF::Elf64_Shdr Shdr{
            .sh_addralign = 1
        };
        int64_t Shndx;

        ELF::Elf64_Shdr* GetSHdr();
        int64_t GetShndx();
        virtual ChunkKind GetKind();
        virtual void UpdateShdr(Context& ctx);
        virtual void CopyBuf(Context& ctx);
    };

    struct ContextArgs
    {
        std::string Output;
        MachineType Emulation = MachineType::None;
        std::vector<std::string> Library;
        std::vector<std::string> LibraryPaths;
        std::vector<std::string> UnrecognizedOption;
    };

    class Context
    {
    public:
        Context();
        ~Context();

        ContextArgs Args{};

        bool ParseArgs(int argc, char** argv);

        bool AppendObjects(rvld::ObjectFile* obj);
        std::vector<rvld::ObjectFile*> GetObjectFiles();
        std::vector<rvld::ObjectFile*> GetLiveObjectFiles();

        MachineType GetMachineType();
        MachineType SetMachineType(MachineType type);

        void AppendChunks(rvld::Chunk* c);
        std::vector<rvld::Chunk*> GetChunks();
        void CollectOutputSections();
        void ComputeSectionSizes();

        // Initialize
        size_t FillUpObjects(std::vector<InputFile*> files);
        void ResolveSymbols();
        void MarkLiveObjects();
        size_t GenerateMergedSection();
        void RegisterSectionPieces();
        void CreateSyntheticSections();
        void ComputeSectionHeaders();
        void ClearUnusedObjectsAndSymbols();

        // Symbol
        bool ExistSymbol(const std::string& name);
        std::pair<std::map<std::string, rvld::Symbol*>::iterator, bool> InsertSymbol(std::string name, rvld::Symbol* symbol);
        std::optional<rvld::Symbol*> GetSymbol(std::string name);

        // Output
        void LinkInputSectionToOutputSection();
        rvld::OutputSection* GetOutputSection(std::string name, uint64_t type, uint64_t flags);
        uint64_t SetOutputSectionsOffset();
        Bytes* AssembleFileContent(size_t filesize);

        // Debug
        void PrintArgs(int argc, char** argv);
        void PrintResolveSymbolMap();
        void PrintMergedSections();

        Bytes* BufferRef();

    private:
        rvld::MergedSection* _GetMergedSectionInstance(std::string name , uint32_t type, uint64_t flags);

        std::vector<rvld::ObjectFile*> _objects{};
        std::map<std::string, rvld::Symbol*> _symbolMap{};
        std::map<std::string, rvld::Symbol*> _resolveSymbolMap{};            // 映射变量 -> 实际定义的文件
        std::map<std::string, rvld::Symbol*> _absSymbolMap{};
        std::vector<rvld::MergedSection*> _mergedSections{};

        rvld::ObjectFile* _internalObjectFile;

        rvld::OutputEHdr* _ehdr;
        rvld::OutputSHdr* _shdr;

        Bytes* _buffer = nullptr;

        std::vector<rvld::OutputSection*> _outputSections{};
        std::vector<Chunk*> _chunks{};
    };

    class SectionFragment
    {
    public:
        MergedSection* OutputSection;
        uint32_t Offset = std::numeric_limits<uint32_t>::max();
        uint32_t P2Align = 1;
        bool IsAlive = true;
    };

    class InputSection
    {
    public:
        InputSection(ObjectFile* file, uint32_t ndx);
        ELF::Elf64_Shdr GetSectionHeader();
        rvld::ObjectFile* GetSourceFile();
        [[nodiscard]] size_t GetSectionIndex() const;
        std::string SectionName();
        uint32_t ShSize() const;
        uint8_t P2Align();
        Bytes Content();

        bool IsAlive() const;
        bool SetAliveStatus(bool status);
        bool IsMergeableSection();

        void LinkOutputSection(OutputSection* osec);

        void SetOffset(uint32_t offset);
        uint32_t GetOffset();

    private:
        ObjectFile* _objectFile;
        uint32_t _sectionIndex;
        uint32_t _offset;
        Bytes _content;
        bool _isAlive = true;
        uint32_t _shSize = 0;
        OutputSection* _outputSection;
    };

    class MergeableSection
    {
    public:
        MergedSection* Parent = nullptr;
        uint8_t P2Align;
        std::string Name;
        ELF::Elf64_Shdr Shdr{};

        void AppendOffset(uint64_t offset);
        void AppendData(Bytes data);
        void AppendFragments(SectionFragment* sf);
        std::pair<SectionFragment*, uint64_t> GetSectionFragment(uint64_t offset);

        std::vector<Bytes> GetData();
        std::vector<uint64_t> GetOffsets();

    private:
        std::vector<Bytes> _data{};
        std::vector<uint64_t> _offsets{};
        std::vector<SectionFragment*> _fragments{};

    };

    class Symbol
    {
    public:
        Symbol(std::string name, ObjectFile* source, size_t symIndex);
        void SetInputSection(InputSection* sec);
        void SetSectionFragment(SectionFragment* sf);
        ELF::Elf64_Sym ELFSymbol();
        ObjectFile* SourceFile();
        ObjectFile* DefinitionFile();
        std::string Name();
        void SetValue(uint64_t v);

    private:
        ObjectFile* _sourceFile = nullptr;
        ObjectFile* _definitionFile = nullptr;
        InputSection* _inputSection = nullptr;
        SectionFragment* _sectionFragment = nullptr;
        std::string _name{};
        uint64_t _value;
        int32_t _symIdx;
    };

    class ElfFile
    {
    public:
        explicit ElfFile(InputFile* file);

        ELF::Elf64_Ehdr ReadELFHeader();

        std::vector<ELF::Elf64_Shdr> GetSectionHeaders();
        std::vector<ELF::Elf64_Shdr> ReadSectionHeaders();

        size_t GetSectionNameStringTableIndex();
        ELF::Elf64_Shdr ReadSectionNameStringTableHeader();
        BytesView ReadSectionNameStringTable();

        ELF::Elf64_Shdr ReadSymbolTableSectionHeader();
        std::vector<ELF::Elf64_Sym> ReadSymbolTable();
        std::string ReadSymbolName(ELF::Elf64_Sym sym);
        bool HasSymbolSection();

        ELF::Elf64_Shdr ReadStringTableSectionHeader();
        BytesView ReadSymbolTableStringTable();

        size_t GetSectionHeaderCount();
        InputFile* GetSourceFile();
        std::string GetName();
        MachineType GetMachineType();

        std::string ReadSectionName(ELF::Elf64_Shdr shdr);
        std::optional<ELF::Elf64_Shdr> FindSectionHeader(ELF::ElfSectionType ty);

        template<typename T>
        std::vector<T> ReadStructVector(uint64_t start, uint64_t num);

        BytesView ReadSectionContent(ELF::Elf64_Shdr shdr);
        BytesView ReadSectionContent(uint16_t index);
        ELF::Elf64_Shdr GetSectionHeader(uint16_t index);
        BytesView ReadContentView(size_t start, size_t size);
        Bytes ReadRowBytesContent(size_t start, size_t size);

    private:
        bool _initialize();

        InputFile* _localFile = nullptr;
        ELF::Elf64_Ehdr _elfHeader;
        std::vector<ELF::Elf64_Shdr> _elfSectionHeaders;
        std::vector<ELF::Elf64_Sym> _elfSymbols;
        BytesView _sectionNameStringTable;

    };

    class ObjectFile : public ElfFile
    {
    public:
        explicit ObjectFile(InputFile* localFile);
        int64_t GetGlobalSymbolStartIndex();
        std::vector<InputSection*> GetInputSections();
        std::vector<Symbol*> Symbols();
        std::vector<Symbol*> GetGlobalSymbols();
        std::vector<MergeableSection*> GetMergeableSections();
        bool HasGlobalSymbol();
        void AppendSymbol(Symbol* sym);
        void RegisterSectionPieces();
        uint32_t GetShnIndex(ELF::Elf64_Sym elf64Sym, size_t index);
        void SetGlobalSymbolIndex(int64_t idx);

    private:
        void _InitializeInputSections();
        void _InitializeSymbols();
        void _InitializeMergeableSections();

        InputSection* _GetInputSection(ELF::Elf64_Sym symbol, size_t index);
        MergeableSection* _SplitSection(InputSection* isec);

        int64_t _symbolTableShndxSectionIdx = -1;                    // section of type SHT_SYMTAB_SHNDX.
        int64_t _globalSymbolStartIndex = -1;

        std::vector<InputSection*> _inputSections;
        std::vector<Symbol*> _symbols;
        std::vector<MergeableSection*> _mergeableSections{};
    };

    class MergedSection : public Chunk
    {
    public:
        MergedSection(std::string name, uint64_t flags, uint32_t type);
        SectionFragment* Insert(const Bytes& key, uint32_t p2align);
        //        void CopyBuf(Bytes& buffer) override;
    private:
        std::map<Bytes, SectionFragment*> _sectionFragments{};
    };

    class OutputEHdr : public Chunk
    {
    public:
        OutputEHdr();

        ChunkKind GetKind() override;
        void CopyBuf(Context& ctx) override;
    };

    class OutputSHdr : public Chunk
    {
    public:
        OutputSHdr();

        ChunkKind GetKind() override;
        void UpdateShdr(Context& ctx) override;
        void CopyBuf(Context& ctx) override;
    };

    class OutputSection : public Chunk
    {
    public:
        OutputSection(std::string name, uint32_t type, uint64_t flags);

        void AppendInputSection(InputSection* isec);
        size_t MembersCount();
        std::vector<InputSection*> ReadMembers();

        ChunkKind GetKind() override;
        void UpdateShdr(Context& ctx) override;
        void CopyBuf(Context& ctx) override;

    private:
        std::vector<InputSection*> _members{};
    };
}
