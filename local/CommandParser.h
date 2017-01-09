//
// Created by alin on 12/31/16.
//
#pragma once
#ifndef SERVER_COMMANDPARSER_H
#define SERVER_COMMANDPARSER_H


#include "../network/ClientHandler.h"
#include  <FileDescription.h>
class ClientHandler;
class CommandParser {
private:
    int socket;
    ClientHandler * parent;
    FileDescription* readFileDescription();
public:
    void setParent(ClientHandler* clientHandler);
    void parseJoin(bool& joined);
    void parsePublish(bool joined);
    void parseQuery(bool joined);
    void parseFind(bool joined);
    void parseUnpublish(bool joined);
    void parseNotify(bool joined);
    void parseBye(bool& joined);

    CommandParser(int socket,ClientHandler* clientHandler);
};


#endif //SERVER_COMMANDPARSER_H
