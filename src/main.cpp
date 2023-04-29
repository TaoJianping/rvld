#include <cstdlib>
#include <format>
#include "rvld.h"
#include <span>
#include <concepts>

int main(int argc, char** argv)
{
    fs::path filePath = "/home/tao/projects/xmake_projects/rvld/tests/out/tests/hello/a.o";

    Linker linker{};

    auto inputFile = linker.NewInputFile(filePath);
    if (!inputFile)
    {
        LOG(ERROR) << "Create File Failed";
        return 1;
    }
    if (!inputFile->IsElfFile())
    {
        LOG(ERROR) << "THIS IS NOT ELF FILE";
        return 1;
    }
    auto objFile = linker.NewObjectFile(inputFile);
    objFile->Parse();
    auto ehdr = objFile->ReadELFHeader();
    auto shdrs = objFile->ReadSectionHeaders();
    auto shstrsectContent = objFile->ReadSectionContent(ehdr.e_shstrndx);

    auto symbols = objFile->ReadSymbolTable();
    auto stringTable = objFile->ReadStringTable();

    for (auto &s : symbols)
    {
        LOG(INFO) << objFile->ReadSymbolName(s);
    }


    delete inputFile;
    delete objFile;

    return 0;
}