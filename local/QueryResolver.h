//
// Created by alin on 1/1/17.
//

#ifndef SERVER_QUERYRESOLVER_H
#define SERVER_QUERYRESOLVER_H


#include <FileDescription.h>
#include "../network/ClientHandler.h"

class QueryResolver {
private:
    ClientHandler* attachedClient;
public:
    QueryResolver(ClientHandler* clientHandler);
    /**
     * need to find some files and then
     * send them to the client via a RESULTS command
     */
    void solveQuery(FileDescription* query);
};


#endif //SERVER_QUERYRESOLVER_H
