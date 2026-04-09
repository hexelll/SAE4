#include "database.h"
#include "list.c"

Connection ConnectionNew(char* host,char* database, char* user, char* passwd,char* port){
    Connection connection;
    connection.arena = ArenaCreate(1024);
    connection.con = PQconnectdb(StringToChar(StringFormatChar(&connection.arena,"user=%s password=%s host=%s port=%s dbname=%s connect_timeout=10",user,passwd,host,port,database),&connection.arena));
    if (PQstatus(connection.con) == CONNECTION_OK) 
        connection.message = StringFrom("",&connection.arena);
    else {
        connection.message = StringFormat(&connection.arena,StringFrom("%s", &connection.arena),PQerrorMessage(connection.con));
    }
    return connection;
}

void ConnectionClose(Connection connection){
    PQfinish(connection.con);
    ArenaDelete(&connection.arena);
}


QueryResult QueryResultExecNew(Connection connection , PGresult* resI){
    QueryResult newResult;
    newResult.res = resI;
    if (PQresultStatus(resI) != PGRES_COMMAND_OK){
        newResult.message = StringFormat(&connection.arena , StringFrom("%s", &connection.arena), PQerrorMessage(connection.con));
    }else{
        newResult.message = StringFrom("", &connection.arena);
    }
    return newResult;
}
QueryResult QueryResultSelectNew(Connection connection , PGresult* res){
    QueryResult newResult;
    newResult.res = res;
    if (PQresultStatus(newResult.res) != PGRES_TUPLES_OK){
        newResult.message = StringFormat(&connection.arena , StringFrom("%s", &connection.arena), PQerrorMessage(connection.con));
    }else{
        newResult.count = PQntuples(res);
        newResult.message = StringFrom("", &connection.arena);
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
    return QueryResultSelectNew(connection, res);
}


//Insert a User in the database, you only need to specify the username.
QueryResult ConnectionInsert(Connection connection,String table, String data){
    String sqlRequest = StringFormat(&connection.arena, StringFrom("Insert into %S values(\'%S\')", &connection.arena), table, data);
    FILE* fp = fopen("./logFile.txt", "w");
    fprintf(fp, StringToChar(sqlRequest, &connection.arena));
    fclose(fp);
    PGresult* res2 = PQexec(connection.con , StringToChar(sqlRequest, &connection.arena));
    PQclear(res2);
    return QueryResultExecNew(connection, res2);
}

QueryResult ConnectionSelect(Connection connection, String SQL){
    FILE* fp = fopen("./logFile.txt", "w");
    fprintf(fp, StringToChar(SQL,&connection.arena));
    fclose(fp);
    PGresult* resRAW = PQexec(connection.con, StringToChar(SQL,&connection.arena));
    return QueryResultSelectNew(connection, resRAW);
}

QueryResult ConnectionExec(Connection connection, String SQL){
    FILE* fp = fopen("./logFile.txt", "w");
    fprintf(fp, StringToChar(SQL,&connection.arena));
    fclose(fp);
    PGresult* resRAW = PQexec(connection.con, StringToChar(SQL,&connection.arena));
    PQclear(resRAW);
    return QueryResultExecNew(connection, resRAW);
}

//Retrive the information provided by the SELECT query provided.
String QueryResultToString(QueryResult query, struct Arena* arena){
    String textRet = StringFrom("", arena);
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

List QueryResultToList(QueryResult query, struct Arena* arena){
    List list = ListNew(arena);
    int rows = PQntuples(query.res);
    int cols = PQnfields(query.res);
    FILE* fp = fopen("./logFile.txt", "w");
    fprintf(fp, StringToChar( StringFromInt(rows, arena), arena));
    fprintf(fp ,"\n");
    fprintf(fp, StringToChar( StringFromInt(cols, arena), arena));
    fclose(fp);
    for(int i=0;i<rows;i++){
        Hashmap* map = ArenaAlloc(arena,sizeof(Hashmap));
        *map = HashmapNew(sizeof(String), arena);
        for(int j=0;j<cols;j++){
            String resText = StringFormat(arena, StringFrom("%s", arena),PQfname(query.res, j));
            String* s = ArenaAlloc(arena,sizeof(String));
            *s = StringFrom((char*)PQgetvalue(query.res, i, j),arena);
            HashmapSet(map, StringToChar(resText, arena),s);
        }
        ListAppendVal(&list,(ListValue){.ptr=map});
    }
    return list;
}