//
// Created by alin on 12/31/16.
//

#ifndef SERVER_COMMANDPARSER_H
#define SERVER_COMMANDPARSER_H


#include "../network/ClientHandler.h"

class CommandParser {
private:
    int socket;
    ClientHandler* parent;
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

};


#endif //SERVER_COMMANDPARSER_H
