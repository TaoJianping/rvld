//
// Created by tjp56 on 2023/5/4.
//

#include <cstring>
#include "Bytes.h"
#include <cassert>



Bytes::Bytes(const std::vector<char> &contents)
{
    for (auto & c: contents)
    {
        push_back(static_cast<std::byte>(c));
    }
}

Bytes::Bytes(BytesView bytesView)
{
    for (auto & c: bytesView)
    {
        push_back(static_cast<std::byte>(c));
    }
}

Bytes Bytes::SubBytes(size_t start, size_t end)
{
    auto beginIndex = this->begin() + start;
    auto endIndex = this->begin() + end;

    Bytes ret{};
    for (;beginIndex < endIndex; beginIndex++)
    {
        ret.push_back(*beginIndex);
    }

    return ret;
}

Bytes Bytes::SubBytes(size_t start)
{
    auto beginIndex = this->begin() + start;
    auto endIndex = this->end();

    Bytes ret{};
    for (;beginIndex < endIndex; beginIndex++)
    {
        ret.push_back(*beginIndex);
    }

    return ret;
}

std::string Bytes::ToString()
{
    std::string str(reinterpret_cast<const char*>(this->data()), this->size());
    return str;
}

void Bytes::Write(size_t pos, const Bytes& bytes)
{
    for (auto byte : bytes)
    {
        assert(pos < size());
        this->at(pos) = byte;
        pos++;
    }
}

void Bytes::ReSize(size_t size)
{
    assert(size <= max_size());
    this->resize(size, std::byte{'\0'});
}
