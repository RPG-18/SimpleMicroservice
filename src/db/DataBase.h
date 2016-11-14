#pragma once

#include <memory>

#include <string>

#include "Snapshot.h"
#include "Iterator.h"

namespace leveldb
{
class DB;
}

namespace DataBase
{

class DataBase
{
public:


    DataBase();

    bool open(const std::string &path2base, bool compression = true);

    bool put(const std::string &key, const ByteArray &value, bool sync = false);
    ByteArray get(const std::string &key);

    Snapshot snapshot();

    Iterator iterator();

private:

    std::shared_ptr<leveldb::DB> m_backend;
};

}
