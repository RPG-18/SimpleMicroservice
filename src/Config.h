#pragma once

#include <string>

namespace MicroService
{

class Config
{
public:


    struct Queue
    {
        std::string name;
    };

    bool load(const std::string &path);

    const std::string &amqpAddress() const noexcept
    {
        return m_address;
    }

    const std::string &localStoragePath() const noexcept
    {
        return m_localStoragePath;
    }

    const Queue &source() const noexcept
    {
        return m_source;
    }

    const Queue &destination() const noexcept
    {
        return m_destination;
    }

private:

    std::string m_address;
    std::string m_localStoragePath;

    Queue m_source;
    Queue m_destination;
};

}