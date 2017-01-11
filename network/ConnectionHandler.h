//
// Created by alin on 12/31/16.
//

#ifndef SERVER_CONNECTIONHANDLER_H
#define SERVER_CONNECTIONHANDLER_H

#include <map>
#include <set>
#include "ClientHandler.h"

#define PORT 5000
class ConnectionHandler {

private:
    static ConnectionHandler * instance;
    int serverSocket;
    std::set<ClientHandler*> clients;
    std::mutex clientsLock;
    std::map<int, ClientHandler*> clientIDs;
    std::vector<std::thread*> threads;
    void _bindSocket();
    void _acceptConnections();
    void acceptConnections();
    ConnectionHandler();
    void cleaner();
    void _cleaner();
public:
    void startService();
    ClientHandler* getClientForId(int id);
    /**
     * checks if there is a un-joined client (connected just for hole punching)
     * with this address, and returns a pointer to it if it is.
     * otherwise retuns a pointer to the joined client with this address.
     * @param address
     * @return
     */
    ClientHandler* getClientConnectedWith(Address address);

    void registerClientId(ClientHandler* client, int id);
    static ConnectionHandler* getInstance();
};


#endif //SERVER_CONNECTIONHANDLER_H
