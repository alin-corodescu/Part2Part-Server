//
// Created by alin on 12/31/16.
//
#pragma once
#ifndef SERVER_CLIENTHANDLER_H
#define SERVER_CLIENTHANDLER_H


#include <queue>
#include <mutex>
#include <Address.h>
#include <thread>
#include "../local/CommandParser.h"
#define CLEANING_INTERVAL 10
#define INACTIVITY_THRESHOLD 10
class CommandParser;
class Command;
class ClientHandler {
private:
    CommandParser * incomingCommandParser;
    int communicationSocket;
    bool joined, connected;
    std::vector<std::thread> threads;
public:
    bool isJoined() const;

private:
    std::queue<Command> commandQueue;
    std::mutex cmdQLock;
    //DB id of the client;
    /**
     * address used by the peers to connect to it;
     */
    Address* addressForPeers;
    /**
     * address used to connect to the server
     */
    Address* connectedFrom;
public:
    Address *getConnectedFrom() const;

public:
    Address *getAddressForPeers() const;

private:
    int cli_id;
public:
    void setCli_id(int cli_id);

public:
    int getCli_id() const;

private:
    std::chrono::steady_clock::time_point last_activity;
    void _listenForCommands();
    void _executeCommand(Command command);
    void _processCommandQueue();
public:
    ClientHandler(int socket);
    void executeCommand(Command command);
    void start();
    std::chrono::steady_clock::time_point lastActive();
    void setAddress(Address* address);
    void addJoinInfo(unsigned int privateIP,unsigned short port);
    void sendPublicIp();

    ~ClientHandler();
};


#endif //SERVER_CLIENTHANDLER_H
