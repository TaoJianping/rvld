//
// Created by tjp56 on 2023/5/4.
//

#include "BytesReader.h"
BytesReader::BytesReader(Bytes& bytes):
    _content(bytes.data(), bytes.size())
{

}

BytesReader::BytesReader(const BytesView& bytes): _content(bytes)
{

}

void BytesReader::Forward(const size_t n)
{
    _pos += n;
}

void BytesReader::Backward(const size_t n)
{
    _pos -= n;
}

std::byte BytesReader::Peek() const
{
    return _content[GetPos()];
}

BytesView BytesReader::PeekRange(const size_t num) const
{
    return BytesView{_content.data() + GetPos(), num};
}

size_t BytesReader::GetPos() const
{
    return _pos;
}

Bytes BytesReader::ReadSection(size_t start, size_t size)
{
//    Bytes bytes {_content.data() + start, size};
    Bytes bytes {};
    bytes.insert(bytes.end(), _content.data() + start, _content.data() + start + size);
    Forward(size);
    return bytes;
}

Bytes BytesReader::ReadSection(size_t size)
{
    Bytes bytes {};
    bytes.insert(bytes.end(), _content.data() + GetPos(), _content.data() + GetPos() + size);
    Forward(size);
    return bytes;
}
