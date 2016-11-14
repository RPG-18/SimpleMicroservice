#include <leveldb/db.h>

#include "easylogging++.h"
#include "DataBase.h"

namespace DataBase
{

using namespace leveldb;

DataBase::DataBase()
    : m_backend(nullptr)
{
}

bool DataBase::open(const std::string &path2base, bool compression)
{
    Options options;
    options.create_if_missing = true;
    options.compression = compression
                          ? kSnappyCompression : kNoCompression;
    DB *database;
    const Status status = DB::Open(options, path2base, &database);
    if (!status.ok()) {
        LOG(WARNING) << "Error opening database"
                     << status.ToString();
        return false;
    }

    m_backend.reset(database);
    return true;
}

bool DataBase::put(const std::string &key, const ByteArray &value, bool sync)
{
    if (m_backend == nullptr) {
        LOG(WARNING) << "The database is not open!";
        return false;
    }

    WriteOptions options;
    options.sync = sync;

    const auto status = m_backend->Put(options, Slice(key), Slice(value.data(), value.size()));

    if (!status.ok()) {
        LOG(ERROR) << "Error writing data"
                   << status.ToString();
    }

    return status.ok();
}

ByteArray DataBase::get(const std::string &key)
{
    if (m_backend == nullptr) {
        LOG(WARNING) << "The database is not open!";
        return ByteArray();
    }

    ReadOptions options;

    ByteArray value;
    const auto status = m_backend->Get(options, Slice(key), &value);
    if (!status.ok() && !status.IsNotFound()) {
        LOG(ERROR) << "Error reading data"
                   << status.ToString();
    }

    return value;
}

Snapshot DataBase::snapshot()
{
    if (m_backend == nullptr) {
        LOG(WARNING) << "The database is not open!";
        return Snapshot();
    }

    return Snapshot(m_backend,
                    m_backend->GetSnapshot());
}

Iterator DataBase::iterator()
{
    const ReadOptions options;
    std::unique_ptr<leveldb::Iterator> rawIterator(m_backend->NewIterator(options));

    return Iterator(std::move(rawIterator));
}

}