//
// Created by alin on 12/31/16.
//

#include <thread>
#include <IOWrappers.h>
#include <cstring>
#include <Command.h>
#include <CommandBuilder.h>
#include <sys/socket.h>
#include "ClientHandler.h"

ClientHandler::ClientHandler(int socket) {
    this->communicationSocket = socket;
    incomingCommandParser = new CommandParser(socket,this);
    addressForPeers = new Address();

}

void ClientHandler::_listenForCommands() {
    char* command;
    while (connected)
    {
        command = readString(communicationSocket,COMM_LENGTH);
        printf("Am primit comanda : -%s-\n",command);
        if (!strcmp(command, commandName(JOIN))) {
            /**
             * this one will have  a reference and update it
             * others will just use to see if it is allowed
             * to do such a thing
             */
            last_activity = std::chrono::steady_clock::now();
            incomingCommandParser->parseJoin(joined);
        }

        if (!strcmp(command, commandName(PUBLISH))) {
            last_activity = std::chrono::steady_clock::now();
            incomingCommandParser->parsePublish(joined);
        }

        if (!strcmp(command, commandName(QUERY))) {
            last_activity = std::chrono::steady_clock::now();
            incomingCommandParser->parseQuery(joined);
        }

        if (!strcmp(command, commandName(FIND))) {
            last_activity = std::chrono::steady_clock::now();
            incomingCommandParser->parseFind(joined);
        }

        if (!strcmp(command, commandName(UNPUBLISH))) {
            last_activity = std::chrono::steady_clock::now();
            incomingCommandParser->parseUnpublish(joined);
        }

        if (!strcmp(command, commandName(NOTIFIY))) {
            //NAT related command
            last_activity = std::chrono::steady_clock::now();
            incomingCommandParser->parseNotify(joined);
        }

        if (!strcmp(command, commandName(BYE))) {
            /**
             * reference on joined aswell
             */
            last_activity = std::chrono::steady_clock::now();
            incomingCommandParser->parseBye(joined);
        }

        if(!strcmp(command, commandName(HEARTBEAT))){
            printf("Parsez heartbeat\n");
            last_activity = std::chrono::steady_clock::now();

        }
        free(command);
    }
}

void ClientHandler::_executeCommand(Command* command) {
    int size;
    size = command->length();
    char *string = (char*) malloc(size);
    command->toString(string);
    writeString(communicationSocket,string, size);
    free(string);
    delete command;
}

void ClientHandler::_processCommandQueue() {
    while (connected)
    {
        std::lock_guard<std::mutex> lockGuard(cmdQLock);
        bool isEmpty = commandQueue.empty();
        while (!isEmpty)
        {
            Command *next = commandQueue.front();
            commandQueue.pop();

            _executeCommand(next);

            isEmpty = commandQueue.empty();

        }
    }
}

void ClientHandler::start() {
    printf("Starting a new client handler\n");
    connected = true;
    incomingCommandParser->setParent(this);
    threads.push_back(new std::thread([=] {_listenForCommands();}));
    threads.push_back(new std::thread([=] {_processCommandQueue();}));

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
    this->connectedFrom = address;

    // this one is null
    addressForPeers->setPublicIP(address->getPublicIP());
}

void ClientHandler::addJoinInfo(unsigned int privateIP, unsigned short port) {
    addressForPeers->setPrivateIP(privateIP);
    addressForPeers->setPrivatePort(port);
    addressForPeers->setPublicPort(port);

    //update connectedFrom with private IP aswell
    connectedFrom->setPrivateIP(privateIP);
}

Address *ClientHandler::getAddressForPeers() const {
    return addressForPeers;
}

void ClientHandler::executeCommand(Command* command) {
    std::lock_guard<std::mutex> lockGuard(cmdQLock);
    commandQueue.push(command);
}

void ClientHandler::sendPublicIp() {
    CommandBuilder builder;
    builder.setType(IDENTITY);
    builder.addArgument(connectedFrom->getPublicIP(),INT);
    executeCommand(builder.build());
}

bool ClientHandler::isJoined() const {
    return joined;
}

Address *ClientHandler::getConnectedFrom() const {
    return connectedFrom;
}

ClientHandler::~ClientHandler() {
    connected = false;
    shutdown(communicationSocket, SHUT_RDWR);
    for (int i = 0 ;i<threads.size(); i++) {
        threads[i]->join();
        //delete threads[i];
    }
    delete incomingCommandParser;
}
