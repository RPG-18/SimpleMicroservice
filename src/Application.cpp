#include <algorithm>

#include <rapidjson/writer.h>

#include "3rdparty/easylogging/easylogging++.h"

#include "db/DataBase.h"
#include "Player.h"
#include "Application.h"

namespace
{
MicroService::Application *APP = nullptr;
}

namespace MicroService
{

Application::Application()
    : m_handler(m_loop)
    , m_localStorage(new DataBase::DataBase)
{
    APP = this;

    m_handlers["player_registered"] = &Application::onPlayerRegistered;
    m_handlers["player_renamed"] = &Application::onPlayerRenamed;
    m_handlers["player_won"] = &Application::onPlayerWon;
}

Application::~Application()
{
    freeMemPlayers();
}

void Application::run()
{
    loadFromLocalStorage();

    m_minuteTimer = m_loop.onInterval(5.0, 60.0, std::bind(&Application::onMinute, this));

    AMQP::TcpConnection connection(&m_handler, AMQP::Address(m_cfg.amqpAddress()));

    createChannel(connection);

    m_loop.onSignal(SIGTERM, [this]() -> bool
    {
        shutdown();
        return false;
    });

    m_loop.onSignal(SIGUSR1, [this]()->bool{
        cleanRating();
        return true;
    });

    m_loop.run();
}

bool Application::onMinute()
{
    calculateRating();
    sendRating();
    return true;
}

void Application::calculateRating()
{
    //todo прикрутить Integer Sort из Boost, для поразрядной сортировки
    std::sort(m_players.begin(), m_players.end(), [](const Player *a, const Player *b)
    {
        return a->points > b->points;
    });
}

void Application::sendRating()
{
    using namespace rapidjson;

    StringBuffer s;
    Writer<StringBuffer> writer(s);
    writer.StartArray();

    const size_t count = std::min(m_players.size(), size_t(10));
    for (size_t i = 0;
         i < count;
         ++i) {
        writer.StartObject();

        writer.Key("id");
        writer.Uint64(m_players[i]->id);

        writer.Key("name");
        writer.String(m_players[i]->name.c_str());

        writer.Key("points");
        writer.Int64(m_players[i]->points);

        writer.EndObject();
    }

    writer.EndArray();
    AMQP::Envelope env(s.GetString());

    m_channel->publish("", m_cfg.destination().name, env);
}

void Application::cleanRating()
{
    for(auto player: m_players)
    {
        player->points = 0;
        updatePlayerInBD(player);
    }
}

void Application::shutdown()
{
    LOG(INFO) << "Stop service!";
    m_minuteTimer->cancel();
    m_loop.stop();
}

bool Application::readConfiguration(const std::string &path)
{
    return m_cfg.load(path);
}

bool Application::openLocalStorage()
{
    return m_localStorage->open(m_cfg.localStoragePath());
}

void Application::createChannel(AMQP::TcpConnection &connection)
{
    m_channel = std::make_unique<AMQP::TcpChannel>(&connection);


    m_channel->declareQueue(m_cfg.source().name, AMQP::durable)
        .onSuccess([&](const std::string &name, uint32_t messagecount, uint32_t consumercount)
                   {
                       LOG(INFO) << "Declared queue "
                                 << name
                                 << ", message count: "
                                 << messagecount;

                       m_channel->consume(m_cfg.source().name)
                           .onReceived([&](const AMQP::Message &message,
                                           uint64_t deliveryTag,
                                           bool redelivered)
                                       {
                                           onMessage(message, deliveryTag, redelivered);
                                       })
                           .onError([](const char *message)
                                    {
                                        LOG(ERROR) << "Error consume:" << message;
                                        APP->shutdown();
                                    });
                   })
        .onError([&](const char *message)
                 {
                     LOG(ERROR) << "Error declare queue:" << message;
                     shutdown();
                 });
}

void Application::onMessage(const AMQP::Message &message,
                            uint64_t deliveryTag,
                            bool redelivered)
{
    parseMessage(message);
    m_channel->ack(deliveryTag);
}

void Application::parseMessage(const AMQP::Message &message)
{
    /*
     * Схемка имеет вид
     * {
     *   "method":"player_registered",
     *   "params":{
     *   ...
     *   }
     * }
     */
    rapidjson::Document doc;
    doc.Parse(message.body(), message.bodySize());

    const std::string method = doc["method"].GetString();
    auto iter = m_handlers.find(method);
    if (iter != m_handlers.end()) {
        iter->second(*this, doc["params"]);
    }
    else {
        LOG(WARNING) << "Unknown method:" << method;
    }
}

void Application::onPlayerRegistered(const JValue &params)
{
    auto obj = params.GetObject();
    const uint64_t playerId = obj["id"].GetUint64();
    if (!isRegistred(playerId)) {
        auto player = new Player;
        player->id = playerId;
        player->name = obj["name"].GetString();
        m_players.push_back(player);
        m_id2player[playerId] = player;
        updatePlayerInBD(player);
    }
}

void Application::onPlayerRenamed(const JValue &params)
{
    auto obj = params.GetObject();
    const uint64_t playerId = obj["id"].GetUint64();
    if (isRegistred(playerId)) {
        auto player = m_id2player[playerId];
        player->name = obj["name"].GetString();
        updatePlayerInBD(player);
    }
    else {
        LOG(WARNING) << "Renaming an unknown user[" << playerId << "]";
    }
}

void Application::onPlayerWon(const JValue &params)
{
    auto obj = params.GetObject();
    const uint64_t playerId = obj["id"].GetUint64();
    if (isRegistred(playerId)) {
        auto player = m_id2player[playerId];
        player->points += obj["points"].GetInt64();
        updatePlayerInBD(player);
    }
    else {
        LOG(WARNING) << "Unknown player[" << playerId << "]";
    }
}

void Application::updatePlayerInBD(const Player *player)
{
    if (!m_localStorage->put(std::to_string(player->id), player->serialize())) {
        LOG(ERROR) << "[" << player->id << ", "
                   << player->name
                   << "] is not updated in the database";
    }
}

void Application::freeMemPlayers()
{
    for (auto iter = m_id2player.begin();
         iter != m_id2player.cend();
         ++iter) {
        delete iter->second;
    }
    m_id2player.clear();
}

bool Application::isRegistred(uint64_t userId) const noexcept
{
    return m_id2player.count(userId) > 0;
}

void Application::loadFromLocalStorage()
{
    auto snapshot = m_localStorage->snapshot();
    auto iter = snapshot.iterator();
    iter.toFirst();
    while (iter.isValid()) {
        auto player = new Player(iter.value());
        m_id2player[player->id] = player;
        m_players.push_back(player);
        iter.next();
    }
}

}