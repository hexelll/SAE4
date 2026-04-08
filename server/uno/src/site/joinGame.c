#include <stdio.h>
#include "/myserver/libs/utils/database.c"
#include "/myserver/libs/server.c"

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
    String* gameCode = HashmapGet(&map,"gameCode");
    err = gameCode ? err : StringConcat(err,StringFrom("missing gameCode in request ",arena),arena);

    if(!userId || !userPwd || !gameCode) {
        response = StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }else {
        QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where code like \"%S\"",*gameCode));
        if (res.message.size == 0) {
            res = ConnectionSelect(con,StringFormatChar(arena,"select * from user where userId like \"%S\" and userPwd like \"%S\"",*userId,*userPwd));
            if (res.message.size == 0) {
                response = StringFormatChar(arena,"{\"ok\":true}");
            }else {
                response = StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
            }
        }else {
            response = StringFormatChar(arena,"{\"ok\":false,\"error\":\"no game with this code\"}");
        }
    }

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
    ConnectionClose(con);
    ArenaDelete(arena);
}