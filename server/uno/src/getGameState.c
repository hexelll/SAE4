#include <stdio.h>
#include "/myserver/libs/server.c"
#include "/myserver/libs/utils/database.c"

int main(int argc,char** argv) {   
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(1024);

    struct Arena* arena = &sarena;

    String response;

    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");
    
    Hashmap map = ServerParseRequest(argv,arena);
    String err = StringFrom("",arena);
    String* userId = HashmapGet(&map,"userId");
    err = userId ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);

    if(!userId || !userPwd) {
        response = StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }else {
        QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S",*userId));
        List tuples = QueryResultToList(res,arena);
        Hashmap user = ListGetVal(&tuples,0)->ptr;
        String* gameId = HashmapGet(user,"gameid");
        if (PQntuples(res.res) > 0 && res.message.size == 0) {
            
        }else {
            response = StringFormatChar(arena,"{\"ok\":false,\"error\":\"no game with this code\"}");
        }
    }

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
    ConnectionClose(con);
    ArenaDelete(arena);

}