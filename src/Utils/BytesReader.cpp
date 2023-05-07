//
// Created by tjp56 on 2023/5/4.
//

#include "BytesReader.h"
BytesReader::BytesReader(Bytes& bytes):
    _content(bytes.data(), bytes.size())
{

}

void BytesReader::Forward(size_t n)
{
    _pos += n;
}

void BytesReader::Backward(size_t n)
{
    _pos -= n;
}

size_t BytesReader::GetPos()
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
