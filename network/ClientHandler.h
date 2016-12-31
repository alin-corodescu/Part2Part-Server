//
// Created by alin on 12/31/16.
//

#ifndef SERVER_CLIENTHANDLER_H
#define SERVER_CLIENTHANDLER_H


#include <queue>
#include <mutex>
#include "../local/CommandParser.h"
#define CLEANING_INTERVAL 10
#define INACTIVITY_THRESHOLD 10
class ClientHandler {
private:
    using namespace std::chrono;

    CommandParser * incomingCommandParser;
    int communicationSocket;
    bool joined, connected;
    std::queue<Command> commandQueue;
    std::mutex cmdQLock;

    steady_clock::time_point last_activity;
    void _listenForCommands();
    void _executeCommand(Command command);
    void _processCommandQueue();
public:
    ClientHandler(int socket);
    void listenForCommands();
    void start();
    steady_clock::time_point lastActive();
};


#endif //SERVER_CLIENTHANDLER_H
