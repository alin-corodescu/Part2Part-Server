#include <iostream>
#include "network/ConnectionHandler.h"

int main() {

    ConnectionHandler *connectionHandler = ConnectionHandler::getInstance();
    connectionHandler->startService();
    return 0;
}