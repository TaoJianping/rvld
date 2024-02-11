//
// Created by tjp56 on 2023/5/3.
//

#ifndef RVLD_ARCHIVE_H
#define RVLD_ARCHIVE_H

#include <filesystem>
#include <utility>

#include "InputFile.h"
#include "BytesReader.h"

struct ArHdr
{
    bool hasPrefix(const std::string& str, const std::string& prefix)
    {
        if (str.substr(0, prefix.size()) == prefix)
        {
            return true;
        }
        return false;
    }

    char name[16];
    char date[12];
    char uid[6];
    char gid[6];
    char mode[8];
    char size[10];
    char end[2];

    [[nodiscard]] bool IsValid() const
    {
        return end[0] == 0x60 && end[1] == 0x0a;
    };

    bool IsSymTab()
    {
        // Symbol Table的前缀是 "/ " 或者 "/SYM64/ "
        auto nameStr = std::string{ name, 16 };
        return hasPrefix(nameStr, "/ ") || hasPrefix(nameStr, "/SYM64/ ");
    }

    bool IsStrTab()
    {
        // String Table的前缀是 "// "
        return name[0] == '/' && name[1] == '/' && name[2] == ' ';
    }

    size_t GetSize()
    {
        return std::stoul(std::string(size, 10));
    }

    std::string ReadName(Bytes& strTab)
    {
        auto nameStr = std::string{ name, 16 };
        if (nameStr.at(0) != '/')
        {
            auto endPos = nameStr.find('/');
            if (endPos != std::string::npos)
            {
                return nameStr.substr(0, endPos);
            }
            else
            {
                return "";
            }
        }
        else
        {
            if (nameStr.at(1) == '/' || nameStr.at(1) == ' ')
            {
                return "";
            }
            else
            {
                auto strTabIndex = std::stoul(nameStr.substr(1, nameStr.size() - 1));
                std::string n{};
                for (;strTabIndex < strTab.size(); strTabIndex++)
                {
                    auto c = static_cast<char>(strTab[strTabIndex]);
                    if (c == '/' && static_cast<char>(strTab[strTabIndex + 1]) == '\n')
                    {
                        break ;
                    }
                    n.push_back(c);
                }
                return n;
            }
        }
    }
};

class Archive
{
public:
    explicit Archive(InputFile* file);
    std::vector<InputFile*> ReadArchiveMembers();

private:
    InputFile* _localFile;
};

#endif //RVLD_ARCHIVE_H
