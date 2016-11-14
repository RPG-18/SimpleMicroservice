#pragma once

#include <memory>
#include <string>

#include "db_types.h"

namespace leveldb
{
class Iterator;
}


namespace DataBase
{

/*!
 * RAII wrapper
 */
class Iterator
{
public:

    Iterator(std::unique_ptr<leveldb::Iterator> rawIterator);
    Iterator(Iterator &&iter);

    /*!
     * Create empty iterator
     */
    Iterator() = default;

    ~Iterator();

    bool isValid() const noexcept;

    void next();

    void prev();

    std::string key();
    ByteArray value();

    /*!
     * Seek to first
     */
    void toFirst();

    /*!
     * Seek to last
     */
    void toLast();

    Iterator(const Iterator &) = delete;
    Iterator &operator=(const Iterator &) = delete;

private:

    std::unique_ptr<leveldb::Iterator> m_iterator;
};

}

