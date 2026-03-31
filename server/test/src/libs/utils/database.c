#include "database.h"

Connection getConnection(String database, String user, String passwd){
    Connection connection;
    connection.arena = ArenaCreate(1024);
    connection.con = PQconnectdb("user=root password=pass4root host=db port=5432 dbname=testdb connect_timeout=10");
    if (PQstatus(connection.con) == CONNECTION_OK) 
        connection.message = StringFrom("connection successfull </br>", &connection.arena);
    else {
        connection.message = StringFormat(&connection.arena,StringFrom("Erreur : %s </br>", &connection.arena),PQerrorMessage(connection.con));
    }
    return connection;
}

void ConnectionClose(Connection connection){
    PQfinish(connection.con);
    ArenaDelete(&connection.arena);
}


QueryResult newQueryResultInsert(Connection connection , PGresult* resI){
    QueryResult newResult;
    newResult.res = resI;
    if (PQresultStatus(resI) != PGRES_COMMAND_OK){
        newResult.message = StringFormat(&connection.arena , StringFrom("Error : %s </br>", &connection.arena), PQerrorMessage(connection.con));
    }else{
        newResult.message = StringFrom("Insertion successfully executed </br>", &connection.arena);
    }
    return newResult;
}
QueryResult newQueryResultSelect(Connection connection , PGresult* res){
    QueryResult newResult;
    newResult.res = res;
    if (PQresultStatus(newResult.res) != PGRES_TUPLES_OK){
        newResult.message = StringFormat(&connection.arena , StringFrom("Error : %s </br>", &connection.arena), PQerrorMessage(connection.con));
    }else{
        newResult.message = StringFrom("Select successfull </br>", &connection.arena);
    }
    return newResult;
}

//Retrieve all from a table.
QueryResult ConnectionGetAll(Connection connection, String table){
    String sqlRequest = StringFormat(&connection.arena, StringFrom("Select * From %S",&connection.arena), table);
    FILE* fp = fopen("./logFile.txt", "w");
    fprintf(fp, StringToChar(sqlRequest, &connection.arena));
    fclose(fp);
    PGresult* res = PQexec(connection.con , StringToChar(sqlRequest, &connection.arena));
    return newQueryResultSelect(connection, res);
}


//Insert a User in the database, you only need to specify the username.
QueryResult ConnectionInsertQuery(Connection connection,String table, String data){
    String sqlRequest = StringFormat(&connection.arena, StringFrom("Insert into %S values(\'%S\')", &connection.arena), table, data);
    FILE* fp = fopen("./logFile.txt", "a");
    fprintf(fp, StringToChar(sqlRequest, &connection.arena));
    fclose(fp);
    PGresult* res2 = PQexec(connection.con , StringToChar(sqlRequest, &connection.arena));
    PQclear(res2);
    return newQueryResultInsert(connection, res2);
}

//Retrive the information provided by the SELECT query provided.
String QueryResultRetrieveinfo(QueryResult query, struct Arena* arena){
    String textRet;
    int rows = PQntuples(query.res);
    int cols = PQnfields(query.res);
    for(int i=0;i<rows;i++) {
        for(int j=0;j<cols;j++) {
            textRet = StringConcat(
                textRet,
                StringFormat(
                    arena,
                    StringFrom("result: %s  </br>",arena),
                    PQgetvalue(query.res,i,j)
                ),
                arena
             );
        }
    }
    PQclear(query.res);
    return textRet;
};



