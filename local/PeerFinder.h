//
// Created by alin on 1/1/17.
//

#ifndef SERVER_PEERFINDER_H
#define SERVER_PEERFINDER_H


#include <FileDescription.h>
#include "../network/ClientHandler.h"

class PeerFinder {
private:
    ClientHandler* attachedClient;
public:
    void findPeersFor(FileDescription* file);
    PeerFinder(ClientHandler* clientHandler);

};


#endif //SERVER_PEERFINDER_H
