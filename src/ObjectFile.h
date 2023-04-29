//
// Created by tjp56 on 2023/4/21.
//

#ifndef RVLD_OBJECTFILE_H
#define RVLD_OBJECTFILE_H

#include "ELFFile.h"

class ObjectFile : public ELF::ElfFile
{
public:
    explicit ObjectFile(InputFile* localFile);
};

#endif //RVLD_OBJECTFILE_H
