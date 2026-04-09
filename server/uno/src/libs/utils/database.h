#include "libpq-fe.h"
#include "string.c"
#include "arena.c"
#include "hashmap.c"
#include "arena.c"
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    PGconn* con;
    String message;
    struct Arena arena;
}Connection;

typedef struct{
    PGresult* res;
    String message;
    int count;
}QueryResult;

//Retreive the connection to the database, if the connection is existing already return this connection.
Connection ConnectionNew(char* host,char* database, char* user, char* passwd,char* port);

QueryResult QueryResultExecNew(Connection connection , PGresult* resI);

QueryResult QueryResultSelectNew(Connection connection , PGresult* res);

//Close the connection to the database.
void ConnectionClose(Connection connection);

//Retrieve all from a table.
QueryResult ConnectionGetAll(Connection connection, String table);

//Insert a User in the database, you only need to specify the username.
QueryResult ConnectionInsert(Connection connection,String table, String data);

//Retrive the information provided by the SELECT query provided.
String QueryResultToString(QueryResult query, struct Arena* arena);

//Execute the query entered in the function, QUERY MUST BE A SELECT QUERY
QueryResult ConnectionSelect(Connection connection, String SQL);

//Execute the query entered in the function, QUERY MUST NOT BE A SELECT
QueryResult ConnectionExec(Connection connection, String SQL);

//Parse a database request and return it in a Hashmap
Hashmap QueryResultToMap(QueryResult query, struct Arena* arena);


