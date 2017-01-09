//
// Created by alin on 1/1/17.
//

#include <CommandBuilder.h>
#include "PeerFinder.h"
#include "DBOperator.h"
#include "../network/ConnectionHandler.h"

void PeerFinder::findPeersFor(FileDescription *file) {
    int file_id = DBOperator::getFileID(file);

    std::vector<int> *clients = DBOperator::getClientsForFile(file_id);

    CommandBuilder *builder = new CommandBuilder();
    builder->setType(REQUEST);
    builder->addArgument(file);

    builder->addArgument(clients->size(),INT);
    int i;
    ConnectionHandler *connectionHandler  = ConnectionHandler::getInstance();
    for (i = 0; i < clients->size(); i++) {
        ClientHandler* client = connectionHandler->getClientForId(clients->at(i));
        Address *address = client->getAddressForPeers();

        builder->addArgument(address->getPublicIP(),INT);
        builder->addArgument(address->getPublicPort(),SHORT);
        builder->addArgument(address->getPrivateIP(),INT);
        builder->addArgument(address->getPrivatePort(),SHORT);
    }

    attachedClient->executeCommand(builder->build());
    delete builder;
}

PeerFinder::PeerFinder(ClientHandler *clientHandler) {
    this->attachedClient = clientHandler;

}
