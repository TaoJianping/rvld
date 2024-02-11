//
// Created by tjp56 on 2023/5/4.
//

#ifndef RVLD_BYTES_H
#define RVLD_BYTES_H

#include <vector>
#include <cstring>
#include <span>
#include <string>


using BytesView = std::span<std::byte>;

class Bytes : public std::vector<std::byte>
{
public:
    Bytes() = default;
    explicit Bytes(const std::vector<char>& contents);
    explicit Bytes(const BytesView& bytesView);

    auto SubBytes(size_t start, size_t end) -> Bytes;
    auto SubBytes(size_t start) -> Bytes;

    void ReSize(size_t size);

    auto Chars() -> const char*;
    auto ToString() -> std::string;

    template<typename T>
    void Write(const size_t pos, T data) {
        const auto size = sizeof(T);
        const auto ptr = reinterpret_cast<char*>(&data);
        for (size_t i = 0; i < size; ++i)
        {
            this->at(pos + i) = static_cast<std::byte>(ptr[i]);
        }
    };

    template<typename T>
    void Write(T data) {
        const auto size = sizeof(T);
        const auto ptr = reinterpret_cast<char*>(&data);
        for (size_t i = 0; i < size; ++i)
        {
            push_back(static_cast<std::byte>(ptr[i]));
        }
    };

    template<typename T>
    void WriteVector(size_t pos, std::vector<T> data) {
        auto size = sizeof(T);
        const auto ptr = reinterpret_cast<char*>(data.data());
        for (size_t i = 0; i < size * data.size(); ++i)
        {
            this->at(pos + i) = static_cast<std::byte>(ptr[i]);
        }
    };

    void Write(size_t pos, const Bytes& bytes);

    template<typename T>
    T ReadStruct(const std::size_t start)
    {
        auto s = T{};
        std::memcpy(&s, data() + start, sizeof(T{}));
        return s;
    };
};

#endif //RVLD_BYTES_H
