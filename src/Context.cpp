//
// Created by tjp56 on 2023/5/1.
//

#include <iostream>
#include <queue>
#include "Context.h"
#include "Archive.h"

namespace po = boost::program_options;

bool Context::ParseArgs(int argc, char** argv)
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

void Context::PrintArgs(int argc, char** argv)
{
    std::cout << "Number of arguments: " << argc << std::endl;
    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }
}

bool Context::AppendObjects(ELF::ObjectFile* obj)
{
    assert(obj != nullptr);
    _objects.push_back(obj);
    return true;
}

std::vector<ELF::ObjectFile*> Context::GetObjectFiles()
{
    return _objects;
}

size_t Context::FillUpObjects(std::vector<InputFile*> files)
{
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
                    auto o = new ELF::ObjectFile(f);
                    AppendObjects(o);
                }
            }
        }
        else if (file->IsElfFile())
        {
            auto of = new ELF::ObjectFile(file);
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
MachineType Context::GetMachineType()
{
    return Args.Emulation;
}

MachineType Context::SetMachineType(MachineType type)
{
    Args.Emulation = MachineType::RISCV64;
    return Args.Emulation;
}

std::vector<ELF::ObjectFile*> Context::GetLiveObjectFiles()
{
    std::vector<ELF::ObjectFile*> ret;
    auto objects = GetObjectFiles();
    for (auto obj : objects) {
        assert(obj != nullptr);
        if (obj->GetSourceFile()->Alive()) {
            ret.push_back(obj);
        }
    }

    return ret;
}
void Context::MarkLiveObjects()
{
    // 这一步是要把所有引用到symbol的文件都设置成alive的
    auto initialFiles = GetLiveObjectFiles();
    std::queue<ELF::ObjectFile*> root;
    for (auto& f : initialFiles) {
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
                    if (!defineFile->GetSourceFile()->Alive()) {
                        auto source = defineFile->GetSourceFile();
                        source->SetAlive();
                        root.push(defineFile);
                    }
                } else {
                    spdlog::error("Not Find symbol define -> {}", gSym->Name());
                }
            }

        }
        root.pop();
    }


}

void Context::ResolveSymbols()
{
    // 这样我就知道所有的已经定义好的symbol
    for (auto object : GetObjectFiles())
    {
        if (!object->HasGlobalSymbol()) {
            continue ;
        }

        for (auto symbol : object->GetGlobalSymbols())
        {
            if (symbol->ELFSymbol().IsUndefinedSymbol()) {
                continue ;
            }

            if (symbol->ELFSymbol().IsAbsSymbol()) {
                spdlog::info("This symbol is abs -> {}", symbol->Name());
                continue ;
            }

            if (ExistSymbol(symbol->Name())) {
                continue ;
            }

            auto [iter, ok] = InsertSymbol(symbol->Name(), symbol);
        }
    }
}

bool Context::ExistSymbol(const std::string& name)
{
    auto iter = _resolveSymbolMap.find(name);
    return iter != _resolveSymbolMap.end();
}

std::pair<std::map<std::string, ELF::Symbol*>::iterator, bool> Context::InsertSymbol(std::string name, ELF::Symbol* symbol)
{
    return _resolveSymbolMap.insert(std::make_pair(name, symbol));
}

std::optional<ELF::Symbol*> Context::GetSymbol(std::string name)
{
    if (!ExistSymbol(name)) {
        return  std::nullopt;
    }

    return std::make_optional(_resolveSymbolMap.at(name));
}
void Context::PrintResolveSymbolMap()
{
    for (auto [key, value] : _resolveSymbolMap) {
        spdlog::info("{} : {}", key, value->SourceFile()->GetName());
    }
}
