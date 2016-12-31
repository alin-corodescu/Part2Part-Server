//
// Created by alin on 12/31/16.
//

#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
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
            clientHandler -> start();
            clients.push_back(clientHandler);
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

        int i;
        clientsLock.lock();
        for (i = 0; i < clients.size();i++)
        {
            steady_clock::time_point now = steady_clock::now();
            duration<double> timeElpased = duration_cast<duration<double>>(now - clients[i]->lastActive());
            if (timeElpased.count() > INACTIVITY_THRESHOLD)
            {
                ClientHandler* inactive = clients[i];
                clients.erase(clients.begin() +  i);
                delete inactive;
            }
        }
        clientsLock.unlock();
        sleep(CLEANING_INTERVAL);
    }

}
