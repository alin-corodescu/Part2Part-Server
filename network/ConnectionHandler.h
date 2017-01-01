//
// Created by alin on 12/31/16.
//

#ifndef SERVER_CONNECTIONHANDLER_H
#define SERVER_CONNECTIONHANDLER_H

#include <map>
#include <set>
#include "ClientHandler.h"

#define PORT 10001
class ConnectionHandler {

private:
    static ConnectionHandler * instance;
    int serverSocket;
    std::set<ClientHandler*> clients;
    std::mutex clientsLock;
    std::map<int, ClientHandler*> clientIDs;
    void _bindSocket();
    void _acceptConnections();
    void acceptConnections();
    ConnectionHandler();
    void cleaner();
    void _cleaner();
public:
    void startService();

    static ConnectionHandler* getInstance();
};


#endif //SERVER_CONNECTIONHANDLER_H
