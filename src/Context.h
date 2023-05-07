//
// Created by tjp56 on 2023/5/1.
//

#ifndef RVLD_CONTEXT_H
#define RVLD_CONTEXT_H

#include <boost/program_options.hpp>
#include "Define.h"
#include "ObjectFile.h"

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
    void PrintArgs(int argc, char** argv);
    bool AppendObjects(ObjectFile* obj);

    std::vector<ObjectFile*> GetObjectFiles();
    MachineType GetMachineType();
    MachineType SetMachineType(MachineType type);
    size_t FillUpObjects(std::vector<InputFile*> files);
    ContextArgs Args{};

private:
    std::vector<ObjectFile*> _objects{};


};

#endif //RVLD_CONTEXT_H
