//
// Created by alin on 12/31/16.
//

#include <IOWrappers.h>
#include <FileDescriptionBuilder.h>
#include "CommandParser.h"
#include "DBOperator.h"
#include "QueryResolver.h"

void CommandParser::parseJoin(bool &joined) {

    unsigned short peerPort;
    unsigned int privateIP;
    readUShort(socket,peerPort);
    readUInt(socket,privateIP);
    if (!joined) {
        joined = true;
        parent->addJoinInfo(privateIP, peerPort);
        parent->setCli_id(DBOperator::addClient());
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
    FileDescriptionBuilder *builder = new FileDescriptionBuilder();
    for (i = 0; i < filesNumber; i++)
    {
        readUInt(socket,fileDescriptionSize);
        char* fileDescriptionString;
        fileDescriptionString = readString(socket,fileDescriptionSize);

        fileDescriptions.push_back(builder->buildFromString(fileDescriptionString));

    }
    delete builder;
    if (joined) {
        DBOperator::addFiles(parent->getCli_id(), fileDescriptions);
    }
    else
    {
        throw "User not joined";
    }
}

void CommandParser::parseQuery(bool joined) {
    unsigned int querySize;

    readUInt(socket,querySize);
    FileDescriptionBuilder *builder = new FileDescriptionBuilder();
    char* queryString;
    queryString = readString(socket,querySize);
    FileDescription* query = builder->buildFromString(queryString);

    QueryResolver *queryResolver = new QueryResolver();

    queryResolver->solveQuery(query);

}
