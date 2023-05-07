//
// Created by tjp56 on 2023/5/4.
//

#include <cstring>
#include "Bytes.h"



Bytes::Bytes(const std::vector<char> &contents)
{
    for (auto & c: contents)
    {
        push_back(static_cast<std::byte>(c));
    }
}

