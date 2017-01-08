cmake_minimum_required(VERSION 3.6)
project(Server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

set(SOURCE_FILES main.cpp network/ConnectionHandler.cpp network/ConnectionHandler.h
        network/ClientHandler.cpp network/ClientHandler.h
        local/CommandParser.cpp local/CommandParser.h
        local/DBOperator.cpp local/DBOperator.h
        local/QueryResolver.cpp local/QueryResolver.h local/PeerFinder.cpp local/PeerFinder.h
        local/sqlite/sqlite3.h)
add_executable(Server ${SOURCE_FILES})

include_directories(/home/alin/Part2Part/shared/include)
set(CMAKE_INCLUDE_PATH "/home/alin/Part2Part/shared")
set(SQLITE3 "/home/alin/Part2Part/server/local/sqlite/libsqlite3.a")
target_link_libraries(Server ${CMAKE_INCLUDE_PATH}/bin/Debug/libIOWrappers.a
        ${CMAKE_INCLUDE_PATH}/bin/Debug/libCommands.a
        ${CMAKE_INCLUDE_PATH}/bin/Debug/libFileDescription.a
        ${CMAKE_INCLUDE_PATH}/bin/Debug/libIntegrityChecker.a
        ${CMAKE_INCLUDE_PATH}/bin/Debug/libNetworkWrappers.a
        ${CMAKE_INCLUDE_PATH}/bin/Debug/libAddress.a
        ${SQLITE3})