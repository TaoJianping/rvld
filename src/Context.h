//
// Created by tjp56 on 2023/5/1.
//

#ifndef RVLD_CONTEXT_H
#define RVLD_CONTEXT_H

#include <boost/program_options.hpp>
#include "Define.h"
#include "ELFFile.h"

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
    bool ParseArgs(int argc, char** argv);
    bool AppendObjects(ELF::ObjectFile* obj);

    std::vector<ELF::ObjectFile*> GetObjectFiles();
    std::vector<ELF::ObjectFile*> GetLiveObjectFiles();
    MachineType GetMachineType();
    MachineType SetMachineType(MachineType type);
    size_t FillUpObjects(std::vector<InputFile*> files);
    ContextArgs Args{};

    void ResolveSymbols();
    void MarkLiveObjects();

    bool ExistSymbol(const std::string& name);
    std::pair<std::map<std::string, ELF::Symbol*>::iterator, bool> InsertSymbol(std::string name, ELF::Symbol* symbol);
    std::optional<ELF::Symbol*> GetSymbol(std::string name);

    void PrintArgs(int argc, char** argv);
    void PrintResolveSymbolMap();

private:
    std::vector<ELF::ObjectFile*> _objects{};
    std::map<std::string, ELF::Symbol*> _symbolMap{};
    std::map<std::string, ELF::Symbol*> _resolveSymbolMap{};            // 映射变量 -> 实际定义的文件

    std::map<std::string, ELF::Symbol*> _absSymbolMap{};            // 映射变量 -> 实际定义的文件

};

#endif //RVLD_CONTEXT_H
