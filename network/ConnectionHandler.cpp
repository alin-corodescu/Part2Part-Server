//
// Created by alin on 12/31/16.
//

#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <Address.h>
#include "ConnectionHandler.h"
#include "ClientHandler.h"

void ConnectionHandler::startService() {
    _bindSocket();
    acceptConnections();
    cleaner();
}

void ConnectionHandler::acceptConnections() {
    //start a new thread to accept connections
    std::thread([=] {_acceptConnections();});
}

void ConnectionHandler::_bindSocket() {
    int val = 1;

    serverSocket = socket(AF_INET,SOCK_STREAM,0);
    setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = PORT;

    if (bind(serverSocket,(struct sockaddr*) &addr,sizeof(struct sockaddr)) == -1)
    {
        throw "Bind failed";
    }

    //now the socket is bound

}

void ConnectionHandler::_acceptConnections() {

    while(1)
    {
        int client;
        struct sockaddr_in clientAddr;
        socklen_t addrLen;
        client = accept(serverSocket,(struct sockaddr *) &clientAddr, &addrLen);
        if (client > 0)
        {
            ClientHandler *clientHandler = new ClientHandler(client);
            Address *address = new Address(clientAddr.sin_addr.s_addr,clientAddr.sin_port);
            clientHandler -> setAddress(address);
            clientHandler -> start();
            /**
             * maybe not here, maybe after a join
             */
            clients.insert(clientHandler);
        }
    }

}

ConnectionHandler::ConnectionHandler() {

}

ConnectionHandler* ConnectionHandler::instance = NULL;

ConnectionHandler *ConnectionHandler::getInstance() {
    if (instance == NULL)
        instance = new ConnectionHandler();
    return instance;
}

void ConnectionHandler::cleaner() {
    std::thread([=] {_cleaner();});
}

void ConnectionHandler::_cleaner() {
    using namespace std::chrono;
    while (1)
    {

        std::set::iterator it;
        clientsLock.lock();
        for (it = clients.begin(); it != clients.end();it++)
        {
            steady_clock::time_point now = steady_clock::now();
            duration<double> timeElpased = duration_cast<duration<double>>(now - (*it)->lastActive());
            if (timeElpased.count() > INACTIVITY_THRESHOLD)
            {
                ClientHandler* inactive = *it;
                clients.erase(it);//might have a problem here
                delete inactive;
            }
        }
        clientsLock.unlock();
        sleep(CLEANING_INTERVAL);
    }

}

ClientHandler *ConnectionHandler::getClientForId(int id) {
    return clientIDs.at(id);
}

ClientHandler *ConnectionHandler::getClientConnectedWith(Address address) {
    using namespace std;

    set<ClientHandler*>::iterator it;
    for (it = clients.begin(); it != clients.end(); it++)
    {
        Address *addr = (*it)->getConnectedFrom();
        //if there is an un-joined peer which is connect from this addr
        if (!(*it)->isJoined())
        {
            if (addr->getPublicIP() == address.getPublicIP() &&
                    addr->getPublicPort() == address.getPublicPort())
            return *it;
        }
    }
    //if there is no un-joined client waiting at this address means
    // that this is a new Notify request, and should find the client with the
    //exact same fields as the parameter
    for (it = clients.begin(); it != clients.end(); it++)
    {
        Address *addr = (*it)->getAddressForPeers();
        if (addr->getPublicIP() == address.getPublicIP() &&
            addr->getPublicPort() == address.getPublicPort() &&
            addr->getPrivateIP() == address.getPrivatePort() &&
            addr->getPrivatePort() == address.getPrivatePort())

            return *it;
    }
    return nullptr;
}