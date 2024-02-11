//
// Created by tjp56 on 2023/5/4.
//

#include <cstring>
#include "Bytes.h"
#include <cassert>
#include "string/strutil.h"


Bytes::Bytes(const std::vector<char> &contents)
{
    for (auto & c: contents)
    {
        push_back(static_cast<std::byte>(c));
    }
}

Bytes::Bytes(const BytesView& bytesView)
{
    for (auto & c: bytesView)
    {
        push_back(c);
    }
}

Bytes Bytes::SubBytes(const size_t start, const size_t end)
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
    return strutil::to_string(this->data());
}

void Bytes::Write(size_t pos, const Bytes& bytes)
{
    for (const auto& byte : bytes)
    {
        assert(pos < size());
        this->at(pos) = byte;
        pos++;
    }
}

void Bytes::ReSize(const size_t size)
{
    assert(size <= max_size());
    this->resize(size, std::byte{'\0'});
}

auto Bytes::Chars() -> const char*
{
    return reinterpret_cast<char*>(data()); ;
}
