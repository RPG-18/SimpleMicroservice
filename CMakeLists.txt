cmake_minimum_required(VERSION 3.6)
project(MicroService)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/3rdparty/rapidjson)
add_subdirectory(src/3rdparty/REACT-CPP)
add_subdirectory(src/3rdparty/AMQP-CPP)

include_directories(src/3rdparty/REACT-CPP
        src/3rdparty/AMQP-CPP
        src/3rdparty/easylogging
        src/3rdparty/rapidjson/include
        src/3rdparty/leveldb/include
        src/db)

add_subdirectory(src/3rdparty/leveldb)
add_subdirectory(src/db)

set(SOURCE_FILES src/3rdparty/easylogging/easylogging++.h
        src/main.cpp
        src/Application.h
        src/Application.cpp
        src/Config.cpp
        src/Config.h
        src/Player.cpp
        src/Player.h)

find_package(Threads REQUIRED)

add_executable(MicroService ${SOURCE_FILES})
target_link_libraries(MicroService
        db
        react-cpp
        amqp-cpp
        ev
        ${CMAKE_THREAD_LIBS_INIT})