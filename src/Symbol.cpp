//
// Created by tjp56 on 2023/6/5.
//

#include "rvld.h"



rvld::Symbol::Symbol(std::string name, rvld::ObjectFile* source, size_t index) :
    _name(std::move(name)), _sourceFile(source), _symIdx(index)
{
}


ELF::Elf64_Sym rvld::Symbol::ELFSymbol()
{
    auto file = SourceFile();
    auto symbols = file->ReadSymbolTable();
    return symbols.at(_symIdx);
}

rvld::ObjectFile* rvld::Symbol::SourceFile()
{
    return _sourceFile;
}

std::string rvld::Symbol::Name()
{
    return _name;
}

rvld::ObjectFile* rvld::Symbol::DefinitionFile()
{
    return _definitionFile;
}

void rvld::Symbol::SetSectionFragment(rvld::SectionFragment* sf)
{
    _sectionFragment = sf;
    _inputSection = nullptr;
}

void rvld::Symbol::SetInputSection(rvld::InputSection* sec)
{
    _inputSection = sec;
    _sectionFragment = nullptr;
}

void rvld::Symbol::SetValue(uint64_t v)
{
    _value = v;
}
