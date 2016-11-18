#pragma once

#include <set>
#include <deque>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

#include <reactcpp.h>
#include <rapidjson/document.h>

#include <amqpcpp.h>
#include <amqpcpp/libev.h>

#include "Config.h"

namespace DataBase
{
class DataBase;
}

namespace MicroService
{
struct Player;

class Application
{
    using JValue = rapidjson::Document::ValueType;
    using ProcessingHadler = std::function<void(Application &, const JValue &)>;
    using ProcessingHadlers = std::unordered_map<std::string, ProcessingHadler>;

public:

    Application();
    ~Application();

    using IntervalWatcherPtr = std::shared_ptr<React::IntervalWatcher>;

    void run();

    bool openLocalStorage();

    void shutdown();

    bool readConfiguration(const std::string &path);

    void onMessage(const AMQP::Message &message,
                   uint64_t deliveryTag,
                   bool redelivered);

private:

    void calculateRating();
    void sendRating();
    void cleanRating();

    void freeMemPlayers();

    bool onMinute();
    bool isRegistred(uint64_t userId) const noexcept;

    void createChannel(AMQP::TcpConnection &connection);
    void parseMessage(const AMQP::Message &message);

    void onPlayerRegistered(const JValue &params);
    void onPlayerRenamed(const JValue &params);
    void onPlayerWon(const JValue &params);

    inline void updatePlayerInBD(const Player *player);

    void loadFromLocalStorage();

private:

    Config m_cfg;
    React::MainLoop m_loop;
    IntervalWatcherPtr m_minuteTimer;
    AMQP::LibEvHandler m_handler;
    std::unique_ptr<DataBase::DataBase> m_localStorage;
    std::unique_ptr<AMQP::TcpChannel> m_channel;
    ProcessingHadlers m_handlers;
    std::vector<std::unique_ptr<Player>> m_players;
    std::unordered_map<uint64_t, Player *> m_id2player;
};

}