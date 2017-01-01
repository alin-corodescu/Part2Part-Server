//
// Created by alin on 12/31/16.
//

#include <IOWrappers.h>
#include <FileDescriptionBuilder.h>
#include <CommandBuilder.h>
#include "CommandParser.h"
#include "DBOperator.h"
#include "QueryResolver.h"
#include "../network/ConnectionHandler.h"
#include "PeerFinder.h"

void CommandParser::parseJoin(bool &joined) {

    unsigned short peerPort;
    unsigned int privateIP;
    readUShort(socket,peerPort);
    readUInt(socket,privateIP);
    if (!joined) {
        joined = true;
        parent->addJoinInfo(privateIP, peerPort);
        parent->setCli_id(DBOperator::addClient());
        parent->sendPublicIp();
    }
    else {
        throw "Already joined";
    }

}

void CommandParser::setParent(ClientHandler* clientHandler) {
    this->parent = clientHandler;
}

void CommandParser::parsePublish(bool joined) {

    unsigned int filesNumber,fileDescriptionSize;
    std::vector<FileDescription*> fileDescriptions;

    readUInt(socket,filesNumber);
    unsigned int i;
    for (i = 0; i < filesNumber; i++)
        fileDescriptions.push_back(readFileDescription());
    if (joined) {
        DBOperator::addFiles(parent->getCli_id(), fileDescriptions);
    }
    else
    {
        throw "User not joined";
    }
}

void CommandParser::parseQuery(bool joined) {

    FileDescription* query = readFileDescription();

    QueryResolver *queryResolver = new QueryResolver(parent);
    if (joined)
        queryResolver->solveQuery(query);
    else
        throw "User not joined";

}

void CommandParser::parseFind(bool joined) {
    FileDescription* fileDescription = readFileDescription();
    PeerFinder *peerFinder = new PeerFinder(parent);

    peerFinder->findPeersFor(fileDescription);

}

FileDescription* CommandParser::readFileDescription() {
    FileDescriptionBuilder *fileDescriptionBuilder = new FileDescriptionBuilder();
    FileDescription *fileDescription;

    unsigned int fdSize;
    readUInt(socket, fdSize);
    char *fileDescriptionString;
    readUInt(socket, fdSize);

    fileDescriptionString = readString(socket, fdSize);
    fileDescription = fileDescriptionBuilder->buildFromString(fileDescriptionString);
    free(fileDescriptionString);

    delete fileDescriptionBuilder;
    return fileDescription;
}

void CommandParser::parseUnpublish(bool joined) {
    unsigned int count;

    readUInt(socket, count);

    unsigned int i;
    for (i =0; i < count; i++)
    {
        FileDescription *fileDescription = readFileDescription();

        if (joined) {
            int fileID = DBOperator::getFileID(fileDescription);
            DBOperator::unpublish(fileID, parent->getCli_id());
        }

    }

}

void CommandParser::parseNotify(bool joined) {
//this should come on a new connection
    //daca atunci cand primesc un notify B, trimit la B un open A => A este adresa de pe care primesc notfiy;
    unsigned int  ip;
    unsigned short port;
    readUInt(socket,ip);
    readUShort(socket,port);

    Address address;
    address.setPublicIP(ip);
    address.setPublicPort(port);

    ConnectionHandler *connectionHandler = ConnectionHandler::getInstance();
    ClientHandler* peer = connectionHandler->getClientAtAddress(address);

    //case when A notifies B to open one port
    CommandBuilder commandBuilder;
    commandBuilder.setType(OPEN);
    commandBuilder.addArgument(parent->getPeerAddress()->getPublicIP());
    commandBuilder.addArgument(parent->getPeerAddress()->getPublicPort());
    Command open = commandBuilder.build();
    peer->executeCommand(open);


}

void CommandParser::parseBye(bool &joined) {
    delete parent;
}
