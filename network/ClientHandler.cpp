//
// Created by alin on 12/31/16.
//

#include <thread>
#include <IOWrappers.h>
#include <cstring>
#include <Command.h>
#include "ClientHandler.h"

ClientHandler::ClientHandler(int socket) {
    this->communicationSocket = socket;

}

void ClientHandler::listenForCommands() {
    //start a new thread to accept connections
    std::thread([=] {_listenForCommands();});

}

void ClientHandler::_listenForCommands() {
    char* command;
    while (1)
    {
        command = readString(communicationSocket,COMM_LENGTH);

        if (!strcmp(command, commandName(JOIN))) {
            /**
             * this one will have  a reference and update it
             * others will just use to see if it is allowed
             * to do such a thing
             */
            last_activity = steady_clock::now();
            incomingCommandParser->parseJoin(joined);
        }

        if (!strcmp(command, commandName(PUBLISH))) {
            last_activity = steady_clock::now();
            incomingCommandParser->parsePublish(joined);
        }

        if (!strcmp(command, commandName(QUERY))) {
            last_activity = steady_clock::now();
            incomingCommandParser->parseQuery(joined);
        }

        if (!strcmp(command, commandName(FIND))) {
            last_activity = steady_clock::now();
            incomingCommandParser->parseFind(joined);
        }

        if (!strcmp(command, commandName(UNPUBLISH))) {
            last_activity = steady_clock::now();
            incomingCommandParser->parseUnpublish(joined);
        }

        if (!strcmp(command, commandName(NOTIFIY))) {
            //NAT related command
            last_activity = steady_clock::now();
            incomingCommandParser->parseNotify(joined);
        }

        if (!strcmp(command, commandName(BYE))) {
            /**
             * reference on joined aswell
             */
            last_activity = steady_clock::now();
            incomingCommandParser->parseBye(joined);
        }

        if(!strcmp(command, commandName(HEARTBEAT))){
            last_activity = steady_clock::now();
        }
        free(command);
    }
}

void ClientHandler::_executeCommand(Command command) {
    int size;
    size = command.length();
    char *string = (char*) malloc(size);
    command.toString(string);
    writeString(communicationSocket,string, size);
    free(string);
}

void ClientHandler::_processCommandQueue() {
    while (connected)
    {
        cmdQLock.lock();
        bool isEmpty = commandQueue.empty();
        cmdQLock.unlock();
        while (!isEmpty)
        {
            cmdQLock.lock();
            Command next = commandQueue.front();
            commandQueue.pop();
            cmdQLock.unlock();

            _executeCommand(next);
            cmdQLock.lock();
            isEmpty = commandQueue.empty();
            cmdQLock.unlock();
        }
    }
}

void ClientHandler::start() {
    incomingCommandParser->setParent(this);
    std::thread([=] {_listenForCommands();});
    std::thread([=] {_processCommandQueue();});

}

std::chrono::steady_clock::time_point ClientHandler::lastActive() {
    return last_activity;
}

int ClientHandler::getCli_id() const {
    return cli_id;
}

void ClientHandler::setCli_id(int cli_id) {
    ClientHandler::cli_id = cli_id;
}

void ClientHandler::setAddress(Address *address) {
    this->peerAddress = address;
}

void ClientHandler::addJoinInfo(unsigned int privateIP, unsigned short port) {
    peerAddress->setPrivateIP(privateIP);
    peerAddress->setPrivatePort(port);
    peerAddress->setPublicPort(port);
}
