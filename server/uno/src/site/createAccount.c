#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/list.c"
#include "../libs/utils/player.c"
#include "/myserver/libs/utils/cards.c"



String makeResponse(struct Arena* arena,Hashmap map, Connection con) {

    String err = StringFrom("",arena);
    String* userName = HashmapGet(&map,"username");
    err = userName ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);

    if(!userName || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    Player* lePlayer;
    lePlayer->id = -1;
    lePlayer->username = userName;
    lePlayer->userPwd = userPwd;
    QueryResult res = InsertPlayer(lePlayer, con);
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Creation of account failed\",\"id\":\"%d\"}", -1);
    }else{
                QueryResult res = ConnectionSelect(con,StringFormatChar(&con.arena,"select max(playerid) from player"));
                int* converr = ArenaAlloc(&con.arena, sizeof(int));
                String idstr = *(String*)HashmapGet(QueryResultToMap(res,&con.arena),"playerid");
                int leID = StringToInt(idstr,converr);
                return StringFormatChar(arena,"{\"ok\":true,\"error\":\"\", \"id\": \"%d\"}", leID);
    }

    return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Query did not get executed\",\"id\":\"%d\"}", -1);
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