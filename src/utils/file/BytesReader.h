//
// Created by tjp56 on 2023/5/4.
//

#ifndef RVLD_BYTESREADER_H
#define RVLD_BYTESREADER_H

#include <span>
#include <cstring>
#include "Bytes.h"

class BytesReader
{
public:
    explicit BytesReader(Bytes& bytes);
    explicit BytesReader(const BytesView& bytes);

    void Forward(size_t n = 1);
    void Backward(size_t n = 1);

    [[nodiscard]] size_t GetPos() const;

    [[nodiscard]] std::byte Peek() const;
    [[nodiscard]] BytesView PeekRange(size_t num) const;

    Bytes ReadSection(size_t start, size_t size);
    Bytes ReadSection(size_t size);

    template<typename T>
    T ReadStruct() {
        auto s = T{};
        const auto len = sizeof(T{});
        std::memcpy(&s, _content.data() + GetPos(), len);
        Forward(len);
        return s;
    };

private:
    std::span<std::byte> _content;
    size_t _pos = 0;
};

#endif //RVLD_BYTESREADER_H
