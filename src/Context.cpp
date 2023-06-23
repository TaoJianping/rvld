//
// Created by tjp56 on 2023/5/1.
//

#include <iostream>
#include <queue>
#include <utility>
#include <boost/program_options.hpp>

#include "Utils/Archive.h"
#include "Utils/Utils.h"
#include "Define.h"
#include "rvld.h"

namespace po = boost::program_options;

bool rvld::Context::ParseArgs(int argc, char** argv)
{
    // Declare the supported options.
    boost::program_options::options_description desc("rvld Option Parser");

    std::string machine;
    //    std::string outputPath;
    std::string plugin;
    std::vector<std::string> pluginOpt;
    bool isStatic{ false };
    bool startGroup;
    bool endGroup;
    //    std::vector<std::string> library;
    //    std::vector<std::string> libraryPath;
    std::string version;
    desc.add_options()("help,h", "Help screen")("version, v", po::value<std::string>(&version), "Version")("plugin", po::value<std::string>(&plugin), "Plugin")("plugin-opt", po::value<std::vector<std::string>>(&pluginOpt), "Plugin-opt")("output, o", po::value<std::string>(&Args.Output), "Output Path")("static", po::bool_switch(&isStatic)->default_value(false), "Is Static")("as-needed", po::bool_switch(), "As Needed")("start-group", po::bool_switch(&startGroup)->default_value(false), "Is Start Group")("end-group", po::bool_switch(&endGroup)->default_value(false), "Is End Group")("-l", po::value<std::vector<std::string>>(&Args.Library), "Library")("-L", po::value<std::vector<std::string>>(&Args.LibraryPaths), "Library Path")("-m", po::value<std::string>(&machine), "specify elf name");

    boost::program_options::command_line_parser parser{ argc, argv };
    parser.options(desc).allow_unregistered().style(
        boost::program_options::command_line_style::unix_style |
        boost::program_options::command_line_style::allow_long_disguise);
    boost::program_options::parsed_options parsed_options = parser.run();
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed_options, vm);
    boost::program_options::notify(vm);

    if (vm.count("-m"))
    {
        if (machine == "elf64lriscv")
        {
            SetMachineType(MachineType::RISCV64);
        }
    }

    std::vector<std::string> unrecognized = po::collect_unrecognized(parsed_options.options, po::include_positional);
    for (const auto& opt : unrecognized)
    {
        Args.UnrecognizedOption.push_back(opt);
    }

    return true;
}

void rvld::Context::PrintArgs(int argc, char** argv)
{
    std::cout << "Number of arguments: " << argc << std::endl;
    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }
}

bool rvld::Context::AppendObjects(rvld::ObjectFile* obj)
{
    assert(obj != nullptr);
    _objects.push_back(obj);
    return true;
}

std::vector<rvld::ObjectFile*> rvld::Context::GetObjectFiles()
{
    return _objects;
}

size_t rvld::Context::FillUpObjects(std::vector<InputFile*> files)
{
//    _internalObjectFile = _CreateInternalObjectFile();
//    AppendObjects(_internalObjectFile);

    for (auto& file : files)
    {
        if (file->IsArchiveFile())
        {
            auto af = Archive{ file };
            auto members = af.ReadArchiveMembers();
            for (auto f : members)
            {
                if (f->IsElfFile())
                {
                    auto o = new rvld::ObjectFile(f);
                    AppendObjects(o);
                }
            }
        }
        else if (file->IsElfFile())
        {
            auto of = new rvld::ObjectFile(file);
            AppendObjects(of);
        }
        else
        {
            spdlog::error("Not Support File -> {}", file->GetPath().c_str());
        }
    }

    //    spdlog::info("End  FillUpObjects");
    return GetObjectFiles().size();
}

MachineType rvld::Context::GetMachineType()
{
    return Args.Emulation;
}

MachineType rvld::Context::SetMachineType(MachineType type)
{
    Args.Emulation = MachineType::RISCV64;
    return Args.Emulation;
}

std::vector<rvld::ObjectFile*> rvld::Context::GetLiveObjectFiles()
{
    std::vector<rvld::ObjectFile*> ret;
    auto objects = GetObjectFiles();
    for (auto obj : objects)
    {
        assert(obj != nullptr);
        if (obj->GetSourceFile()->Alive())
        {
            ret.push_back(obj);
        }
    }

    return ret;
}
void rvld::Context::MarkLiveObjects()
{
    // 这一步是要把所有引用到symbol的文件都设置成alive的
    auto initialFiles = GetLiveObjectFiles();
    std::queue<rvld::ObjectFile*> root;
    for (auto& f : initialFiles)
    {
        root.push(f);
    }

    while (!root.empty())
    {
        auto file = root.front();
        auto globalSymbols = file->GetGlobalSymbols();

        for (auto gSym : globalSymbols)
        {
            if (gSym->ELFSymbol().IsUndefinedSymbol())
            {
                auto resolveSymbol = GetSymbol(gSym->Name());
                if (resolveSymbol.has_value())
                {
                    auto v = resolveSymbol.value();
                    auto defineFile = v->SourceFile();
                    if (!defineFile->GetSourceFile()->Alive())
                    {
                        auto source = defineFile->GetSourceFile();
                        source->SetAlive();
                        root.push(defineFile);
                    }
                }
                else
                {
                    spdlog::error("Not Find symbol define -> {}", gSym->Name());
                }
            }
        }
        root.pop();
    }

    spdlog::info("Mark {} live object files", GetLiveObjectFiles().size());
}

void rvld::Context::ResolveSymbols()
{
    // 这样我就知道所有的已经定义好的symbol
    for (auto object : GetObjectFiles())
    {
        if (!object->HasGlobalSymbol())
        {
            continue;
        }

        for (auto symbol : object->GetGlobalSymbols())
        {
            if (symbol->ELFSymbol().IsUndefinedSymbol())
            {
                continue;
            }

            if (symbol->ELFSymbol().IsAbsSymbol())
            {
//                spdlog::info("This symbol is abs -> {}", symbol->Name());
                continue;
            }

            if (ExistSymbol(symbol->Name()))
            {
                continue;
            }

            auto [iter, ok] = InsertSymbol(symbol->Name(), symbol);
        }
    }
}

bool rvld::Context::ExistSymbol(const std::string& name)
{
    auto iter = _resolveSymbolMap.find(name);
    return iter != _resolveSymbolMap.end();
}

std::pair<std::map<std::string, rvld::Symbol*>::iterator, bool> rvld::Context::InsertSymbol(std::string name, rvld::Symbol* symbol)
{
    return _resolveSymbolMap.insert(std::make_pair(name, symbol));
}

std::optional<rvld::Symbol*> rvld::Context::GetSymbol(std::string name)
{
    if (!ExistSymbol(name))
    {
        return std::nullopt;
    }

    return std::make_optional(_resolveSymbolMap.at(name));
}

void rvld::Context::PrintResolveSymbolMap()
{
    for (auto [key, value] : _resolveSymbolMap)
    {
        spdlog::info("{} : {}", key, value->SourceFile()->GetName());
    }
}

size_t rvld::Context::GenerateMergedSection()
{
    auto objs = GetObjectFiles();
    for (auto obj : objs)
    {
        auto mergeableSections = obj->GetMergeableSections();
        for (auto ms : mergeableSections)
        {
            if (ms == nullptr) {
                continue ;
            }
            auto name = ms->Name;
            auto type = ms->Shdr.sh_type;
            auto flags = ms->Shdr.sh_flags;
            ms->Parent = _GetMergedSectionInstance(name, type, flags);
        }
    }

    spdlog::info("Generate {} MergedSection", _mergedSections.size());
    return _mergedSections.size();
}

rvld::MergedSection* rvld::Context::_GetMergedSectionInstance(std::string name, uint32_t type, uint64_t flags)
{
    auto handledName = Utils::GetOutputName(name, flags);
//    spdlog::info("Get Output Name And Handle Name -> {} : {}", name, handledName);
    uint64_t mask = ~(SHF_GROUP | SHF_MERGE | SHF_STRINGS | SHF_COMPRESSED);
    flags &= mask;

    for (auto ms : _mergedSections)
    {
        if (ms->Name == handledName && ms->Shdr.sh_type == type && ms->Shdr.sh_flags == flags)
        {
            return ms;
        }
    }

    auto ms = new rvld::MergedSection{ handledName, flags, type };
//    spdlog::info("Generate MergedSection -> {}", handledName);
    _mergedSections.push_back(ms);
    return ms;
}

void rvld::Context::PrintMergedSections()
{
    for (auto&& ms : _mergedSections)
    {
//        spdlog::info("--------------------------");
        spdlog::info("section name -> {}", ms->Name);
    }
}

void rvld::Context::RegisterSectionPieces()
{
    for (auto&& obj : GetObjectFiles())
    {
        obj->RegisterSectionPieces();
    }
}

void rvld::Context::CreateSyntheticSections()
{
    _ehdr = new OutputEHdr{};
    _shdr = new OutputSHdr{};

    AppendChunks(_ehdr);
    AppendChunks(_shdr);
}

void rvld::Context::ComputeSectionHeaders()
{
    int64_t shndx = 1;
    for (auto chunk : GetChunks())
    {
        if (chunk->GetKind() != ChunkKind::HEADER)
        {
            chunk->Shndx = shndx++;
        }
    }
}

void rvld::Context::AppendChunks(Chunk* c)
{
    _chunks.push_back(c);
}

uint64_t rvld::Context::SetOutputSectionsOffset()
{

    uint64_t fileoff = 0;
    auto chunks = GetChunks();
    for (const auto & c : chunks)
    {
        fileoff = Utils::AlignTo(fileoff, c->GetSHdr()->sh_addralign);
        c->GetSHdr()->sh_offset = fileoff;
        fileoff += c->GetSHdr()->sh_size;
    }

    return fileoff;
}

std::vector<rvld::Chunk*> rvld::Context::GetChunks()
{
    return _chunks;
}

rvld::OutputSection* rvld::Context::GetOutputSection(std::string name, uint64_t type, uint64_t flags)
{
    name = Utils::GetOutputName(name, flags);
    flags = flags & ~static_cast<uint64_t>(SHF_GROUP) &
            ~static_cast<uint64_t>(SHF_COMPRESSED) &
            ~static_cast<uint64_t>(SHF_LINK_ORDER);

    for (auto osec : _outputSections)
    {
        if (name == osec->Name && type == osec->Shdr.sh_type && flags == osec->Shdr.sh_flags)
        {
            return osec;
        }
    }

    auto osec = new rvld::OutputSection{name, static_cast<uint32_t>(type), flags};
    _outputSections.push_back(osec);
    return osec;
}

void rvld::Context::LinkInputSectionToOutputSection()
{
    size_t isecSize = 0;

    for (auto obj : GetObjectFiles())
    {
        for (auto isec : obj->GetInputSections())
        {
            if (isec == nullptr)
            {
                continue ;
            }
            isecSize++;
            auto o = GetOutputSection(isec->SectionName(), isec->GetSectionHeader().sh_type, isec->GetSectionHeader().sh_flags);
            assert(o != nullptr);
            isec->LinkOutputSection(o);
            if (isec->IsAlive() && isec->GetSourceFile()->GetSourceFile()->Alive())
            {
                o->AppendInputSection(isec);
            }
        }
    }

    spdlog::info("Read InputSection Size is {}", isecSize);
}

void rvld::Context::CollectOutputSections()
{
    for (auto osec : _outputSections)
    {
        if (osec->MembersCount() > 0)
        {
            _chunks.push_back(osec);
        }
    }
}

void rvld::Context::ComputeSectionSizes()
{
    size_t sectionSize = 0;

    for (auto osec : _outputSections)
    {
        uint64_t offset = 0;
        uint64_t P2Align = 0;

        sectionSize += osec->ReadMembers().size();
        for (auto isec : osec->ReadMembers())
        {
            offset = Utils::AlignTo(offset, 1 << P2Align);
            isec->SetOffset(offset);
            offset = offset + isec->ShSize();
            P2Align = std::max(P2Align, static_cast<uint64_t>(isec->P2Align()));
        }

        osec->Shdr.sh_size = offset;
        osec->Shdr.sh_addralign = 1 << P2Align;
    }

    spdlog::info("OutputSections Size is {}", _outputSections.size());
    spdlog::info("Compute Section Size is {}", sectionSize);
}

Bytes* rvld::Context::AssembleFileContent(size_t filesize)
{
    auto buffer = new Bytes{};
    buffer->resize(filesize, std::byte{'\0'});

//    for (auto chunk : GetChunks())
//    {
//        chunk->CopyBuf(<#initializer #>);
//    }
    return nullptr;
}

Bytes* rvld::Context::BufferRef()
{
    return _buffer;
}

rvld::Context::Context()
{
    _buffer = new Bytes{};
}

rvld::Context::~Context()
{
    delete _buffer;
    delete _ehdr;
    delete _shdr;
}

void rvld::Context::ClearUnusedObjectsAndSymbols()
{
    auto pos = std::remove_if(_objects.begin(), _objects.end(), [](const auto& item) {
        return !item->GetSourceFile()->Alive();
    });

    _objects.erase(pos, _objects.end());
}
