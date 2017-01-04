//
// Created by alin on 1/1/17.
//

#ifndef SERVER_DBOPERATOR_H
#define SERVER_DBOPERATOR_H


#include <vector>
#include <FileDescription.h>

class DBOperator {
private:
public:
    /**
     * sets up the tables
     * in memory tables
     */
    static void createTables();
    /**
     * @param fileID - DB id of the file being searched
     * @return vector of cli_ID's of the clients offering that file
     */
    static std::vector<int>* getClientsForFile(int fileID);

    static int getFileID(FileDescription *fileDescription);

    /**
     * adds a client to the database and returns the cli_id
     * @return
     */
    static int addClient();

    /**
     * adds the files to the database for the clientID
     * updates where files already exist
     * @param clientID
     * @param files
     */
    static void addFiles(int clientID,const std::vector<FileDescription*>& files);

    /**
     * the entry from the publications table
     * @param clientID
     * @param fileID
     */
    static void unpublish(int clientID,int fileID);

};


#endif //SERVER_DBOPERATOR_H
