//
// Created by tjp56 on 2023/5/4.
//

#ifndef RVLD_BYTESREADER_H
#define RVLD_BYTESREADER_H

#include <gsl/span>
#include <cstring>
#include "Bytes.h"

class BytesReader
{
public:
    explicit BytesReader(Bytes &bytes);
    void Forward(size_t n = 1);
    void Backward(size_t n = 1);
    Bytes ReadSection(size_t start, size_t size);
    Bytes ReadSection(size_t size);
    size_t GetPos();

    template<typename T>
    T ReadStruct() {
        auto s = T{};
        auto len = sizeof(T{});
        std::memcpy(&s, _content.data() + GetPos(), len);
        Forward(len);
        return s;
    };

private:
    gsl::span<std::byte> _content;
    size_t _pos = 0;
};

#endif //RVLD_BYTESREADER_H
