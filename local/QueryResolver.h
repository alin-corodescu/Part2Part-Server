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
    void solveQuery(FileDescription* query);
};


#endif //SERVER_QUERYRESOLVER_H
