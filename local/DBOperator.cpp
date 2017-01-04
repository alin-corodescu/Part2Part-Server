//
// Created by alin on 1/1/17.
//

#include <cstring>
#include <FileDescriptionBuilder.h>
#include "DBOperator.h"
#define SQL_MAX 1000
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\t", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int getIDScallback(void* idVector, int argc, char **argv, char **azColName){
    using namespace std;
    vector<int>* ids = (vector<int>*) idVector;

    int i;
    for(i=0; i<argc; i++){
        ids->push_back(atoi(argv[i]));
        //printf("%s = %s\t", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    //printf("\n");
    return 0;
}
static int FileDescriptionscallback(void* fdVector, int argc, char **argv, char **azColName){
    using namespace std;
    vector<FileDescription*>* ids = (vector<FileDescription*>*) fdVector;

    int i;
    FileDescriptionBuilder builder;
    builder.init();
    for(i=0; i<argc; i++){
        if (!strcmp(azColName[i],"NAME"))
            builder.addName(argv[i]);
        if (!strcmp(azColName[i],"SIZE"))
            builder.addSize(atoi(argv[i]));
        if (!strcmp(azColName[i],"HASH"))
            builder.addHash(argv[i]);
        if (!strcmp(azColName[i],"DESCRIPTION"))
            builder.addDescription(argv[i]);
        if (!strcmp(azColName[i],"TYPE"))
            builder.addType(argv[i]);
    }

    ids->push_back(builder.build());
    return 0;
}

int DBOperator::addFile(FileDescription* fileDescription)
{
    char sql[SQL_MAX];
    int size;
    std::string name,hash,description,type;
    size = fileDescription->getFileSize();
    name = fileDescription->getFileName();
    hash = fileDescription->getHash();
    description = fileDescription->getDescription();
    type = fileDescription->getType();

    sprintf(sql,"INSERT INTO FILES (SIZE, NAME, HASH, DESCRIPTION,TYPE) VALUES (%d, '%s', '%s', '%s', '%s');",size,name.data(),
    hash.data(),description.data(),type.data());

    char * errmsg;
    sqlite3_exec(db,sql,callback,NULL, &errmsg);

    return (int)sqlite3_last_insert_rowid(db);
}
void createClientsTable(sqlite3*& db)
{
    {
        const char* sql;
        char* zErrMsg = 0;

        sql = "CREATE TABLE CLIENTS("  \
         "ID_CLI INTEGER PRIMARY KEY NOT NULL" \
         "ZERO INTEGER);";
        int rc;
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }else{
            fprintf(stdout, "Clients Table created successfully\n");
        }

    }
}
void createFilesTable(sqlite3*& db)
{
    {
        const char* sql;
        char* zErrMsg = 0;

        sql = "CREATE TABLE FILES("  \
         "ID_FILE INTEGER PRIMARY KEY NOT NULL," \
         "HASH TEXT," \
         "NAME TEXT," \
         "DESCRIPTION TEXT," \
         "SIZE INTEGER," \
         "TYPE TEXT );";

        int rc;
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }else{
            fprintf(stdout, "Files Table created successfully\n");
        }
    }
}
void createPublicationsTable(sqlite3* db)
{
    {
        const char* sql;
        char* zErrMsg = 0;

        sql = "CREATE TABLE PUBLICATIONS("  \
         "ID_FILE INTEGER REFERENCES FILES(ID_FILE)," \
         "ID_CLI INTEGER REFERENCES CLIENTS(ID_CLI) ON DELETE CASCADE);";

        int rc;
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }else{
            fprintf(stdout, "Publications Table created successfully\n");
        }
    }
}
void openDataBase(sqlite3*& db)
{
    int rc;
    rc = sqlite3_open(NULL,&db);
    char * zErrMsg = 0;
    const char* fk = "PRAGMA foreign_keys = ON";
    sqlite3_exec(db, fk, callback, NULL, &zErrMsg);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
}

void DBOperator::createTables() {
    openDataBase(db);
    createClientsTable(db);
    createFilesTable(db);
    createPublicationsTable(db);

}

std::vector<int> *DBOperator::getClientsForFile(int fileID) {
    using namespace std;
    static vector<int> clients;
    clients.clear();
    char sql[SQL_MAX];
    char * errmsg;

    sprintf(sql,"SELECT ID_CLI FROM PUBLICATIONS WHERE ID_FILE = %d;",fileID);

    sqlite3_exec(db,sql,getIDScallback,(void*) &clients, &errmsg);

    return &clients;

}

int DBOperator::getFileID(FileDescription *fileDescription) {
    using namespace std;
    int size = fileDescription->getFileSize();
    string hash = fileDescription->getHash();
    string name = fileDescription->getFileName();
    char sql[SQL_MAX];
    sprintf(sql,"SELECT ID_FILE FROM FILES WHERE NAME = '%s' AND HASH = '%s' AND SIZE = %d ;",name.data(), hash.data(), size);
    vector<int> id;
    char * errmsg;

    sqlite3_exec(db,sql,getIDScallback,(void*) &id, &errmsg);
    if (id.size() != 0)
        return id[0];
    else return 0;
}

int DBOperator::addClient() {

    const char* sql = "INSERT INTO CLIENTS (ZERO) VALUES (0);";
    char * errmsg;
    sqlite3_exec(db,sql,callback,NULL, &errmsg);
    return (int) sqlite3_last_insert_rowid(db);
}

void DBOperator::unpublish(int clientID, int fileID) {
    using namespace std;
    char sql[SQL_MAX];

    sprintf(sql,"DELETE FROM PUBLICATIONS WHERE ID_CLI = %d AND ID_FILE = %d ;",clientID,fileID);
    char * errmsg;
    sqlite3_exec(db,sql,callback,NULL,&errmsg);

    sprintf(sql,"SELECT ID_FILE FROM PUBLICATIONS WHERE ID_FILE = %d;",fileID);
    vector<int> id;

    sqlite3_exec(db,sql,getIDScallback,(void*) &id,&errmsg);

    if (id.size() == 0)
    {
        sprintf(sql,"DELETE FROM FILES WHERE ID_FILE = %d;",fileID);
        sqlite3_exec(db,sql,callback,NULL,&errmsg);
    }

}

void DBOperator::addFiles(int clientID, const std::vector<FileDescription *> &files) {
    int i;
    char sql[SQL_MAX];
    char * errmsg;
    for (i = 0; i < files.size(); i++)
    {
        int id = getFileID(files[i]);
        if (!id)
            id = addFile(files[i]);
        sprintf(sql,"INSERT INTO PUBLICATIONS (ID_CLI, ID_FILE) VALUES (%d, %d);",clientID,id);
        sqlite3_exec(db,sql,callback,NULL,&errmsg);
    }

}

void DBOperator::findFiles(const char *condition, std::vector<FileDescription *> &ids) {
    char sql[SQL_MAX];
    char * errmsg;

    sprintf(sql,"SELECT * FROM FILES WHERE %s;",condition);

    sqlite3_exec(db,sql,FileDescriptionscallback,(void*) &ids,&errmsg);
}

