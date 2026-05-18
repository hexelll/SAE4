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
}QueryResult;

//Retreive the connection to the database, if the connection is existing already return this connection.
Connection getConnexion(String database, String user, String passwd);

QueryResult newQueryResult(Connection connection, PGresult* res,  char* ErrorMessage);

//Close the connection to the database.
void ConnectionClose(Connection connection);

//Retrieve all from a table.
QueryResult ConnectionGetAll(Connection connection, String table);

//Insert a User in the database, you only need to specify the username.
QueryResult ConnectionInsertQuery(Connection connection,String table,String data);

//Retrive the information provided by the SELECT query provided.
String QueryResultRetrieveinfo(QueryResult query, struct Arena* arena);

QueryResult ConnectionRawQuery(Connection connection, String SQL);


