#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"

String makeResponse(struct Arena* arena,Connection con,Hashmap map) {
    
    String err = StringFrom("",arena);
    String* userId = HashmapGet(&map,"userId");
    err = userId ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);
    String* gameCode = HashmapGet(&map,"gameCode");
    err = gameCode ? err : StringConcat(err,StringFrom("missing gameCode in request ",arena),arena);
    if(!userId || !userPwd || !gameCode) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where code = '%S'",*gameCode));
    if (!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no game with this code\"}");
    }

    List tuples = QueryResultToList(res,arena);
    String* gameId = (String*)HashmapGet(ListGetVal(&tuples,0)->ptr,"gameid");

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S and userpwd = '%S'",*userId,*userPwd));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where joinedgameid = %S and playerid = %S",*gameId,*userId));
    if(res.count > 0) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"user already joined this game\"}");
    }

    res = ConnectionSelect(con,StringFormatChar(arena,"select max(gameindex) as newindex from player where joinedgameid = %S and not (playerid = %S)",*gameId,*userId));
    int gameIndex = 0;
    int* converr = ArenaAlloc(arena,sizeof(int));
    if(res.count > 0) {
        List gameIndexTuples = QueryResultToList(res,arena);
        String* gameIndexStr = (String*)HashmapGet(ListGetVal(&gameIndexTuples,0)->ptr,"newindex");
        if (gameIndexStr && gameIndexStr->size > 0) {
            gameIndex = StringToInt(*gameIndexStr,converr)+1;
        }
    }
    
    res = ConnectionExec(con,StringFormatChar(arena,"update player set joinedgameid = %S, gameindex = %d where playerid = %S",*gameId,gameIndex,*userId));
    if(res.message.size != 0) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"an error occured in insertion\"}");
    }

    return StringFormatChar(arena,"{\"ok\":true}");
}

int main(int argc,char** argv) {
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(1024);

    struct Arena* arena = &sarena;

    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");

    String response = makeResponse(arena,con,ServerParseRequest(argv,arena));

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);

    ConnectionClose(con);
    ArenaDelete(arena);
}