#include <stdio.h>
#include "/myserver/libs/server.c"
#include "/myserver/libs/utils/database.c"

int main(int argc,char** argv) {   
    ServerInitDefault();
/*
    
    Hashmap map = ServerParseRequest(argv,&sarena);
    String text = StringFormat(&sarena,StringFrom("url: <a href=%s>%s</a> <br> arguments: <ul>",&sarena),argv[2],argv[2]);
    MapKeys keys = HashmapKeys(&map);
    for(int i=0;i<keys.size;i++) {
        char* key = keys.keys[i];
        String value = *(String*)HashmapGet(&map,key);
        text = StringConcat(text,StringFormat(&sarena,StringFrom("\n<li>%s=%S</li>",&sarena),key,value),&sarena);
    }
    text = StringConcat(text,StringFrom("</ul>",&sarena),&sarena);
    
    PGconn *con = PQconnectdb("user=root password=pass4root host=db port=5432 dbname=testdb connect_timeout=10");
    int id_user;
    char* name;

    if (PQstatus(con) == CONNECTION_OK) 
        text = StringConcat(text,StringFrom("The bluetooth connection is established<br>",&sarena),&sarena);
    else {
        text = StringConcat(text,StringFrom("db connection error<br>",&sarena),&sarena);
    }

        PGresult *res = PQexec(con, StringToChar(StringFormat(&sarena,StringFrom("SELECT * FROM %S", &sarena),*(String*)HashmapGet(&map,"table")),&sarena));
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        text = StringConcat(text,StringFormat(&sarena,StringFrom("SELECT * FROM %s", &sarena),*(String*)HashmapGet(&map,"table")), &sarena);
        text = StringConcat(text,StringFormat(&sarena,StringFrom("db query error %s. <br>",&sarena),PQerrorMessage(con)),&sarena);
    }

    int rows = PQntuples(res);
    int cols = PQnfields(res);
    for(int i=0;i<rows;i++) {
        for(int j=0;j<cols;j++) {
            text = StringConcat(
                text,
                StringFormat(
                    &sarena,
                    StringFrom("result: %s  </br>",&sarena),
                    PQgetvalue(res,i,j)
                ),
                &sarena
             );
        }
    }
    PQclear(res);
    

        PGresult *res2 = PQexec(con, StringToChar(StringFormat(&sarena,StringFrom("Insert INTO %S (username) VALUES (\'%S\')",&sarena),*(String*)HashmapGet(&map,"sectable"),*(String*)HashmapGet(&map,"user")),&sarena));
        if (PQresultStatus(res2) != PGRES_COMMAND_OK) {
            text = StringConcat(text,StringFormat(&sarena,StringFrom("db insert error %s<br>",&sarena),PQerrorMessage(con)),&sarena);
        }
        else {
            text = StringConcat(text,StringFrom("db insert success<br>",&sarena),&sarena);
        }
        PQclear(res2);
        

    //PQclear(res);

    PQfinish(con);

    */

    struct Arena sarena = ArenaCreate(1024);
    Connection laConnection = getConnection(StringFrom("db", &sarena), StringFrom("root", &sarena),StringFrom("pass4root", &sarena));
    String text = laConnection.message;

    QueryResult result = ConnectionGetAll(laConnection, StringFrom("testuser", &sarena));

    text = StringConcat(text, result.message, &sarena);

    QueryResult res2 = ConnectionInsertQuery(laConnection, StringFrom("testuser(username)",&sarena), StringFrom("john Doe", &sarena));

    text = StringConcat(text, res2.message, &sarena);

    text = StringConcat(text, QueryResultRetrieveinfo(result, &sarena),  &sarena);


    ServerRespond(200,StringFrom("{\"Content-Type\":\"text/html\"}",&sarena),text,&sarena);

    ConnectionClose(laConnection);

    ArenaDelete(&sarena);

}