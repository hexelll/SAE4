#include <stdio.h>
#include "/myserver/libs/server.c"
#include "/myserver/libs/utils/database.c"

int main(int argc,char** argv) {   
    ServerInitDefault();

    struct Arena sarena = ArenaCreate(1024);
    Connection laConnection = getConnection(StringFrom("db", &sarena), StringFrom("root", &sarena),StringFrom("pass4root", &sarena));
    String text = laConnection.message;

    QueryResult result = ConnectionRawQuerySelect(laConnection, StringFrom("Select * From testuser", &sarena));

    text = StringConcat(text, result.message, &sarena);

    //QueryResult res2 = ConnectionRawQueryInsert(laConnection, StringFrom("Insert into testuser(username) values('lakaka')",&sarena));

    //text = StringConcat(text, res2.message, &sarena);

    text = StringConcat(text, QueryResultRetrieveinfo(result, &sarena),  &sarena);


    ServerRespond(200,StringFrom("{\"Content-Type\":\"text/html\"}",&sarena),text,&sarena);

    ConnectionClose(laConnection);

    ArenaDelete(&sarena);

}