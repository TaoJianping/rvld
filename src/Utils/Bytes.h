//
// Created by tjp56 on 2023/5/4.
//

#ifndef RVLD_BYTES_H
#define RVLD_BYTES_H

#include <vector>
#include <gsl/span>
#include <cstring>

using BytesView = gsl::span<std::byte>;


class Bytes : public std::vector<std::byte>
{
public:
    Bytes() = default;
    explicit Bytes(const std::vector<char>& contents);
    explicit Bytes(BytesView bytesView);

    Bytes SubBytes(size_t start, size_t end);
    Bytes SubBytes(size_t start);

    void ReSize(size_t size);

    std::string ToString();

    template<typename T>
    void Write(size_t pos, T data) {
        auto size = sizeof(T);
        auto ptr = reinterpret_cast<char*>(&data);
        for (size_t i = 0; i < size; ++i)
        {
            this->at(pos + i) = static_cast<std::byte>(ptr[i]);
        }
    };

    template<typename T>
    void Write(T data) {
        auto size = sizeof(T);
        auto ptr = reinterpret_cast<char*>(&data);
        for (size_t i = 0; i < size; ++i)
        {
            push_back(static_cast<std::byte>(ptr[i]));
        }
    };

    template<typename T>
    void WriteVector(size_t pos, std::vector<T> data) {
        auto size = sizeof(T);
        auto ptr = reinterpret_cast<char*>(data.data());
        for (size_t i = 0; i < size * data.size(); ++i)
        {
            this->at(pos + i) = static_cast<std::byte>(ptr[i]);
        }
    };

    void Write(size_t pos, const Bytes& bytes);

    template<typename T>
    T ReadStruct(std::size_t start)
    {
        auto s = T{};
        std::memcpy(&s, data() + start, sizeof(T{}));
        return s;
    };
};

#endif //RVLD_BYTES_H
