file(GLOB REACT_CPP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dns/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dns/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/watchers/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(react-cpp ${REACT_CPP_SOURCES})
