//
// Created by tjp56 on 2023/5/1.
//

#include <iostream>
#include "Context.h"
#include "glog/logging.h"
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
    desc.add_options()
        ("help,h", "Help screen")
        ("version, v", po::value<std::string>(&version), "Version")
        ("plugin", po::value<std::string>(&plugin), "Plugin")
        ("plugin-opt", po::value<std::vector<std::string>>(&pluginOpt), "Plugin-opt")
        ("output, o", po::value<std::string>(&Args.Output), "Output Path")
        ("static", po::bool_switch(&isStatic)->default_value(false), "Is Static")
        ("as-needed", po::bool_switch(), "As Needed")
        ("start-group", po::bool_switch(&startGroup)->default_value(false), "Is Start Group")
        ("end-group", po::bool_switch(&endGroup)->default_value(false), "Is End Group")
        ("-l", po::value<std::vector<std::string>>(&Args.Library), "Library")
        ("-L", po::value<std::vector<std::string>>(&Args.LibraryPaths), "Library Path")
        ("-m", po::value<std::string>(&machine), "specify elf name");

    boost::program_options::command_line_parser parser{argc, argv};
    parser.options(desc).allow_unregistered().style(
        boost::program_options::command_line_style::unix_style |
        boost::program_options::command_line_style::allow_long_disguise);
    boost::program_options::parsed_options parsed_options = parser.run();
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed_options, vm);
    boost::program_options::notify(vm);

//    std::cout << desc << std::endl;

    if (vm.count("-m")) {
        LOG(INFO) << "Find machine ->" << machine << std::endl;
        if (machine == "elf64lriscv") {
            SetMachineType(MachineType::RISCV64);
        }
    }
//
//    if (vm.count("output")) {
//        LOG(INFO) << "Find Output Path -> " << outputPath << std::endl;
//        Args.Output = outputPath;
//    }
//
//    if (vm.count("plugin")) {
//        LOG(INFO) << "plugin path ->" << plugin;
//    }
//
//    if (vm.count("plugin-opt")) {
//        for (auto &o : pluginOpt) {
//            LOG(INFO) << "plugin opt -> " << o;
//        }
//    }
//
//    if (vm.count("-l")) {
//        for (auto &s : library) {
//            LOG(INFO) << "Library ->" << s;
//        }
//    }
//
//    if (vm.count("static")) {
//        LOG(INFO) << "Static Mode is On";
//    }
//
//    if (vm.count("start-group")) {
//        if (startGroup)
//            LOG(INFO) << "Detect Start Group";
//    }
//
//    if (vm.count("end-group")) {
//        if (endGroup)
//            LOG(INFO) << "Detect End Group";
//    }
//
//    if (vm.count("-L")) {
//        for (auto &s : libraryPath) {
//            LOG(INFO) << "Library Path ->" << s;
//            Args.LibraryPaths.push_back(s);
//        }
//    }

    std::vector<std::string> unrecognized = po::collect_unrecognized(parsed_options.options, po::include_positional);
    // LOG(INFO) << "Unrecognized options:";
    for (const auto& opt : unrecognized) {
//         LOG(INFO) << "unrecognized option -->" << opt;
         Args.UnrecognizedOption.push_back(opt);
    }

    return true;
}

void Context::PrintArgs(int argc, char** argv)
{
    std::cout << "Number of arguments: " << argc << std::endl;
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }
}


bool Context::AppendObjects(ObjectFile* obj)
{
    _objects.push_back(obj);
    return true;
}

std::vector<ObjectFile*> Context::GetObjectFiles()
{
    return _objects;
}

size_t Context::FillUpObjects(std::vector<InputFile*> files)
{
    for (auto& file : files)
    {
        if (file->IsArchiveFile())
        {
            auto af = Archive{file};
            auto members = af.ReadArchiveMembers();
            for (auto f : members)
            {
                if (f->IsElfFile())
                {
                    AppendObjects(new ObjectFile(f));
                }
            }
        }
        else if (file->IsElfFile())
        {
            AppendObjects(new ObjectFile(file));
        }
        else
        {
            LOG(INFO) << "Not Support Type";
        }
    }

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
