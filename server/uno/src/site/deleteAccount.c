#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/list.c"
#include "../libs/utils/player.c"
#include "/myserver/libs/utils/cards.c"



String makeResponse(struct Arena* arena,Hashmap map, Connection con) {

    String err = StringFrom("",arena);
    String* userid = HashmapGet(&map,"userId");
    err = userid ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);

    if(!userid || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    int* converr = ArenaAlloc(&con.arena, sizeof(int));
    int leUserId = StringToInt(*userid, converr);
    String query = StringFormatChar(arena,"select * from player where playerid = %d and userpwd = '%S'",leUserId,*userPwd);
    QueryResult resCheck1 = ConnectionSelect(con,query);
    if(!(resCheck1.count > 0 && resCheck1.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }


    

    QueryResult res = PlayerDelete(leUserId, con);
    
    if(!(res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}", res.message);
    }else{
                return StringFormatChar(arena,"{\"ok\":true,\"error\":\"\"}");
    }
    
    return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Query did not get executed\"}");
}

int main(int argc,char** argv) {
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(1024);

    struct Arena* arena = &sarena;

    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");

    String response = makeResponse(arena,ServerParseRequest(argv,arena), con);

    ConnectionClose(con);

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
    ArenaDelete(arena);
}