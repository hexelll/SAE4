#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/player.c"

String makeResponse(struct Arena* arena,Connection con,Hashmap map) {
    int* converr = ArenaAlloc(arena,sizeof(int));

    String err = StringFrom("",arena);
    String* userId = HashmapGet(&map,"userId");
    err = userId ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);
    if(!userId || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }
    int playerId = StringToInt(*userId,converr);
    Player player = PlayerFindById(playerId,con);

    if(player.gameIndex == NULL) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"user isn't in a game\"}");
    }

    int gameIndex = *player.gameIndex;

    if(!player.joinedGameId) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"user isn't in a game\"}");
    }

    if(player.createdGameId && *player.createdGameId == *player.joinedGameId) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"user is a game creator\"}");
    }

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where gameid = %d",*player.joinedGameId));

    if(res.count == 0 && res.message.size > 0) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"this user's game has been deleted\"}");
    }

    Hashmap* gameMap = QueryResultToMap(res,arena);

    String* currentPlayerIndex = HashmapGet(gameMap,"currentplayerindex");
    if(currentPlayerIndex && currentPlayerIndex->size > 0) {
        return StringFrom("{\"ok\":false,\"error\":\"can't quit started game\"}",arena);
    }
    List players = GetPlayersInGame(*player.joinedGameId,con);
    
    ListRemoveNode(&players,gameIndex);

    ConnectionExec(con,StringFormatChar(arena,"update player set joinedgameid = NULL,gameIndex = NULL where playerid = %d",player.id));

    for(int i=0;i<players.size;i++) {
        Player* p = ListGetPtr(&players,i);
        ConnectionExec(con,StringFormatChar(arena,"update player set gameIndex = %d where playerid = %d",i,p->id));
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