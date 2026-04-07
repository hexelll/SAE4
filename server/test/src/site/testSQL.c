#include <stdio.h>
#include "/myserver/libs/server.c"
#include "/myserver/libs/utils/database.c"

int main(int argc,char** argv) {   
    ServerInitDefault();


    

    struct Arena sarena = ArenaCreate(1024);
    Connection laConnection = getConnection(StringFrom("db", &sarena), StringFrom("root", &sarena),StringFrom("pass4root", &sarena));
    String text = laConnection.message;

    


    

    QueryResult result2 = ConnectionRawQueryInsert(laConnection, StringFrom("Insert into autretestuser(username) values ('test')",&sarena));

    text = StringConcat(text, result2.message, &sarena);

    QueryResult result = ConnectionRawQuerySelect(laConnection, StringFrom("Select * From testuser", &sarena));

    Hashmap leMap = QueryResultRetrieveinfoHash(result, &sarena);
    text = StringConcat(text, StringFrom("</br></br> resultat des hashmaps : </br>", &sarena),&sarena);

    MapKeys keys = HashmapKeys(&leMap);
    for(int i=0;i<keys.size;i++) {
        char* key = keys.keys[i];
        Hashmap line = *(Hashmap*)HashmapGet(&leMap,key);
        MapKeys lineMapKeys = HashmapKeys(&line);
        text = StringConcat(text, StringFormat(&sarena,StringFrom("\n<h1>%s</h1>", &sarena), key), &sarena);
        for(int j=0;j<lineMapKeys.size;j++){
            char* theKey = lineMapKeys.keys[j];
            String value = *(String*)HashmapGet(&line,theKey);
            text = StringConcat(text,StringFormat(&sarena,StringFrom("\n<li>%s=%S</li>",&sarena),theKey,value),&sarena);
        }   
    }


    //QueryResult res2 = ConnectionRawQueryInsert(laConnection, StringFrom("Insert into testuser(username) values('lakaka')",&sarena));

    //text = StringConcat(text, res2.message, &sarena);

    ServerRespond(200,StringFrom("{\"Content-Type\":\"text/html\"}",&sarena),text,&sarena);

    ConnectionClose(laConnection);

    ArenaDelete(&sarena);

}