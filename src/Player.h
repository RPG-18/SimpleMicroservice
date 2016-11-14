#pragma once

#include <ctime>
#include <cstdint>

#include "db/db_types.h"

namespace MicroService
{

struct Player
{
    Player();

    /*!
     * deserialize
     *
     */
    Player(const DataBase::ByteArray &data);

    uint64_t id;
    std::string name;

    int64_t points;
    DataBase::ByteArray serialize() const;
};

}