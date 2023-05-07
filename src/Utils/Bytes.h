//
// Created by tjp56 on 2023/5/4.
//

#ifndef RVLD_BYTES_H
#define RVLD_BYTES_H

#include <vector>
#include <gsl/span>

using BytesView = gsl::span<std::byte>;


class Bytes : public std::vector<std::byte>
{
public:
    Bytes() = default;
    explicit Bytes(const std::vector<char>& contents);

    template<typename T>
    T ReadStruct(std::size_t start)
    {
        auto s = T{};
        std::memcpy(&s, data() + start, sizeof(T{}));
        return s;
    };
};

#endif //RVLD_BYTES_H
