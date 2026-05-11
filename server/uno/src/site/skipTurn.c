#include "../libs/utils/database.c"
#include "../libs/utils/cards.c"
#include "../libs/server.c"

String makeResponse(char** argv,Connection con,struct Arena* arena) {
    int* converr = ArenaAlloc(arena,sizeof(int));
    Hashmap map = ServerParseRequest(argv,arena);
    String err = StringFrom("",arena);
    String* userId = HashmapGet(&map,"userId");
    err = userId ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);
    if(!userId || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S and userpwd like '%S'",*userId,*userPwd));
    if (!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id\"}");
    }
    
    

    Hashmap* player = QueryResultToMap(res,arena);
    String joinedGameId = *(String*)HashmapGet(player,"joinedgameid");
    if(joinedGameId->size > 0) {
        res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where gameid = %S",joinedGameId));
    }else {
        return StringFrom("{\"ok\":false,\"error\":\"player isn't in a game\"}");
    }
    
    Hashmap* game = QueryResultToMap(res,arena);
    
    int isStarted = ((String*)HashmapGet(game,"currentplayerindex"))->size != 0;

    int gameId = StringToInt(*(String*)HashmapGet(game,"gameid"),converr);

    int gameIndex = StringToInt(*(String*)HashmapGet(game,"currentplayerindex"),converr);

    

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where joinedgameid = %d order by gameindex asc",gameId));
    List players = QueryResultToList(res,arena);
    

    Hashmap* currentplayer = ListGetVal(&players,gameIndex)->ptr;
    
    int currentplayerid = StringToInt(*(String*)HashmapGet(currentplayer,"playerid"),converr);
    if (currentplayerid != StringToInt(*userId,converr)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"it isn't this player's turn\"}");
    }

    int isReversed = StringToInt(*(String*)HashmapGet(game,"isreversed"),converr);

    int k = isReversed ? -k : k;

    gameIndex = (gameIndex+k) % players.size;
    gameIndex = gameIndex < 0 ? gameIndex+players.size : gameIndex;

    ConnectionExec(con,StringFormatChar(arena,"update game set isreversed = %d, currentplayerindex = %d where gameid = %d",isReversed,gameIndex,gameId));
    return StringFrom("{\"ok\":true}",arena);
}

int main(int arc,char** argv) {
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(1024);

    struct Arena* arena = &sarena;

    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");

    ServerRespond(200,StringFrom("{\"Content-type\":\"application/json\"}",arena),makeResponse(argv,con,arena),arena);
    ConnectionClose(con);
    ArenaDelete(arena);
}