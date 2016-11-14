#pragma once

#include <memory>

#include "Iterator.h"

namespace leveldb
{
class DB;
class Snapshot;
}

namespace DataBase
{

/*!
 * RAII wrapper
 */
class Snapshot
{
public:

    Snapshot();

    ~Snapshot();

    ByteArray get(const std::string &key);

    Iterator iterator();

private:

    Snapshot(const std::weak_ptr<leveldb::DB> &backend, const leveldb::Snapshot *snapshot);

private:

    friend class DataBase;

    std::weak_ptr<leveldb::DB> m_backend;
    const leveldb::Snapshot *m_shapshot;
};

}


