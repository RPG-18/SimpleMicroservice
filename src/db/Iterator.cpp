#include <leveldb/db.h>

#include "easylogging++.h"
#include "Iterator.h"

namespace DataBase
{

Iterator::Iterator(std::unique_ptr<leveldb::Iterator> rawIterator)
    : m_iterator(std::move(rawIterator))
{
}

Iterator::~Iterator()
{
    if (!m_iterator->status()
        .ok()) {
        LOG(ERROR) << "Error when working with iterator:"
                   << m_iterator->status().ToString();
    }
}

Iterator::Iterator(Iterator &&iter)
    : m_iterator(std::move(iter.m_iterator))
{

}

bool Iterator::isValid() const noexcept
{
    if (m_iterator == nullptr) {
        return false;
    }

    return m_iterator->Valid();
}

void Iterator::next()
{
    if (m_iterator != nullptr) {
        m_iterator->Next();
    }
}

void Iterator::prev()
{
    if (m_iterator != nullptr) {
        m_iterator->Prev();
    }
}

std::string Iterator::key()
{
    if (m_iterator == nullptr) {
        return std::string();
    }

    const auto string = m_iterator->key()
        .ToString();

    return string;
}

ByteArray Iterator::value()
{
    if (m_iterator == nullptr) {
        return ByteArray();
    }

    const auto slice = m_iterator->value();
    return ByteArray(slice.data(), slice.size());
}

void Iterator::toFirst()
{
    if (m_iterator != nullptr) {
        m_iterator->SeekToFirst();
    }
}

void Iterator::toLast()
{
    if (m_iterator != nullptr) {
        m_iterator->SeekToLast();
    }
}

}
