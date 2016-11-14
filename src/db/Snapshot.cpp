#include <cassert>
#include <leveldb/db.h>

#include "easylogging++.h"
#include "Snapshot.h"

namespace DataBase
{
using namespace leveldb;

Snapshot::Snapshot()
    : m_shapshot(nullptr)
{
}

Snapshot::Snapshot(const std::weak_ptr<leveldb::DB> &backend, const leveldb::Snapshot *snapshot)
    : m_backend(backend), m_shapshot(snapshot)
{
}

Snapshot::~Snapshot()
{
    if (auto spt = m_backend.lock()) {
        spt->ReleaseSnapshot(m_shapshot);
        m_shapshot = nullptr;
    }
    else {
        LOG(ERROR) << "The database is deleted. Dangling reference on snapshot!";
        assert(0);
    }
}

ByteArray Snapshot::get(const std::string &key)
{
    if (auto spt = m_backend.lock()) {
        ReadOptions options;
        options.snapshot = m_shapshot;

        ByteArray value;
        const auto status = spt->Get(options, Slice(key), &value);
        if (!status.ok()) {
            LOG(ERROR)
                << "Error reading data from snapshot"
                << status.ToString();
        }

        return value;

    }
    else {
        LOG(ERROR) << "The database is deleted. Dangling reference on snapshot!";
        return ByteArray();
    }
}

Iterator Snapshot::iterator()
{
    if (auto spt = m_backend.lock()) {
        ReadOptions options;
        options.snapshot = m_shapshot;
        std::unique_ptr<leveldb::Iterator> rawIterator(spt->NewIterator(options));

        return Iterator(std::move(rawIterator));

    }
    else {
        LOG(ERROR) << "The database is deleted. Dangling reference on snapshot!";
        return Iterator();
    }
}

}