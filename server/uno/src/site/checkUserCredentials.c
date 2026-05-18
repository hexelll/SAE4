#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/list.c"
#include "/myserver/libs/utils/cards.c"

List getAllPlayersByGameId(int gameId , struct Arena* arena, Connection theconnection){
    QueryResult resAllPlayers = ConnectionSelect(theconnection, StringFormatChar(arena, "select * from player where joinedgameid = %d", gameId));
    if(!(resAllPlayers.count||resAllPlayers.message.size))
    return QueryResultToList(resAllPlayers, arena);
}

int getPlayerGameId(String playerid, struct Arena* arena, Connection theconnection){
    QueryResult resPlayers = ConnectionSelect(theconnection, StringFormatChar(arena, "select * from player where playerid =%S", playerid));
    if(!(resPlayers.count > 0 || resPlayers.message.size == 0)){
        return -1;
    }
    List tuples = QueryResultToList(resPlayers,arena);
    Hashmap* user = ListGetVal(&tuples,0)->ptr;
    String* gameId = HashmapGet(user,"joinedgameid");
    int gameIdNumber = -1;
    return StringToInt(*gameId , &gameIdNumber);
}

String makeResponse(struct Arena* arena,Hashmap map, Connection con) {

    String err = StringFrom("",arena);
    String* username = HashmapGet(&map,"username");
    err = username ? err : StringConcat(err,StringFrom("missing username in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);

    if(!username || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }
    
    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select playerid from player where username like '%S' and userpwd like '%S'",*username,*userPwd));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this username and password\"}");
    }
    String id = *(String*)HashmapGet(QueryResultToMap(res,arena),"playerid");
    return StringFormatChar(arena,"{\"ok\":true,\"id\":%S}",id);
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