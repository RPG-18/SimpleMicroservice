#include <iostream>
#include <fstream>
#include <rapidjson/document.h>

#include "Config.h"

namespace MicroService
{

bool Config::load(const std::string &path)
{
    std::ifstream ifs;
    ifs.open(path);

    if (!ifs.is_open()) {
        return false;
    }

    std::string buff;

    ifs.seekg(0, std::ios::end);
    const auto fileSize = ifs.tellg();
    buff.assign(fileSize, ' ');
    ifs.seekg(0, std::ios::beg);
    ifs.read(&buff[0], fileSize);

//todo исользовать json-schema для валидации конфига
    rapidjson::Document doc;
    doc.Parse(buff.c_str(), buff.size());

    m_address = doc["address"]
        .GetString();

    m_localStoragePath = doc["local_storage"]
        .GetString();

    {
        const auto source = doc["source"].GetObject();
        m_source.name = source["queue"].GetString();
    }
    {
        const auto source = doc["destination"].GetObject();
        m_destination.name = source["queue"].GetString();
    }

    return true;
}

}