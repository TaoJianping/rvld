//
// Created by tjp56 on 2023/5/3.
//

#include "Archive.h"

Archive::Archive(InputFile* file) :
    _localFile(file)
{
}

std::vector<InputFile*> Archive::ReadArchiveMembers()
{
    fs::path absolutePath = fs::absolute(_localFile->GetPath());
    std::vector<InputFile*> ret{};
    auto content = _localFile->GetContents();
    auto reader = BytesReader(content);

    auto magicLen = std::strlen(ARCHIVE_MAGIC);
    reader.Forward(magicLen);

    auto count = 0;
    Bytes stringTable{};
    while (content.size() - reader.GetPos() > 1)
    {
        if (reader.GetPos() % 2 == 1)
        {
            reader.Forward();
        }

        auto ah = reader.ReadStruct<ArHdr>();
        if (!ah.IsValid())
        {
            spdlog::error("Not Valid ArHdr");
            //            LOG(ERROR) << "Identifier: " << std::string(ah.name, 16);
            //            LOG(ERROR) << "Timestamp: " << std::string(ah.date, 12);
            //            LOG(ERROR) << "Owner ID: " << std::string(ah.uid, 6);
            //            LOG(ERROR) << "Group ID: " << std::string(ah.gid, 6);
            //            LOG(ERROR) << "File mode: " << std::string(ah.mode, 8);
            //            LOG(ERROR) << "Size: " << std::string(ah.size, 10);
            break;
        }

        auto contentBytes = reader.ReadSection(ah.GetSize());

        if (ah.IsSymTab())
        {
//            spdlog::info("Find Symbol Table");
        }

        if (ah.IsStrTab())
        {
//            spdlog::info("Find String Table");
            stringTable.insert(stringTable.begin(), contentBytes.begin(), contentBytes.end());
        }

        count++;

        // TODO 有些没释放
        auto file = new InputFile{ ah.ReadName(stringTable), contentBytes, _localFile, false };
        if (file->GetFileType() == FileType::ELF && file->GetElfFileType() == ELF::ElfType::REL)
        {
            ret.push_back(file);
        }
        else
        {
            delete file;
        }
    }
    return ret;
}
