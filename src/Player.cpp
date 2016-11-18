#include <sstream>

#include "Player.h"

namespace MicroService
{

Player::Player()
    : id(0)
    , points(0)
{
}

Player::Player(uint64_t id, const std::string& name)
    : id(id)
    , name(name)
    , points(0)
{
}

Player::Player(const DataBase::ByteArray &data)
{
//todo выпилить stringstream для оптимизации
    std::stringstream ss(data);
    ss.read(reinterpret_cast<char *>(&id), sizeof(id));

    uint16_t nameLen = 0;
    ss.read(reinterpret_cast<char *>(&nameLen), sizeof(nameLen));
    name.resize(nameLen);

    ss.read(&name[0], name.size());
    ss.read(reinterpret_cast<char *>(&points), sizeof(points));
}

DataBase::ByteArray Player::serialize() const
{
//todo выпилить stringstream для оптимизации

    std::stringstream ss;
    ss.write(reinterpret_cast<const char *>(&id), sizeof(id));
    const uint16_t nameLen = name.size();
    ss.write(reinterpret_cast<const char *>(&nameLen), sizeof(nameLen));

    ss.write(name.data(), name.size());
    ss.write(reinterpret_cast<const char *>(&points), sizeof(points));

    return ss.str();
}

}