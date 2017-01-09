//
// Created by alin on 1/1/17.
//

#include <CommandBuilder.h>
#include "QueryResolver.h"
#include "DBOperator.h"
#define MAX_LENGTH 500
QueryResolver::QueryResolver(ClientHandler *clientHandler) {
    this->attachedClient = clientHandler;

}

void QueryResolver::solveQuery(FileDescription *query) {
    // find files in the db with names or description
    using namespace std;
    vector<FileDescription*> matchings;
    char condition[MAX_LENGTH];

    sprintf(condition,"NAME LIKE '%s' OR DESCRIPTION LIKE '%s'",query->getFileName().data(),query->getDescription().data());

    DBOperator::findFiles(condition,matchings);

    CommandBuilder builder;
    builder.setType(RESULTS);
    builder.addArgument((unsigned int)matchings.size(),INT);
    for (int i = 0; i < matchings.size(); i++) {
       builder.addArgument(matchings[i]);
    }
    Command results = builder.build();

    attachedClient->executeCommand(results);


}
