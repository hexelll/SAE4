#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/cards.c"
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

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S and userpwd like '%S'",*userId,*userPwd));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }
    
    List creatorTuple = QueryResultToList(res,arena);
    if(!(creatorTuple.size > 0)){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not convert to list\"}");
    }

    Hashmap* creator = ListGetVal(&creatorTuple,0)->ptr;
    if(!(creator || creator->size > 0)){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not retrieve supposed creator\"}");
    }

    int gameId = StringToInt(*(String*)HashmapGet(creator,"createdgameid"),converr);

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where gameid = %d",gameId));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no game created by this user\"}");
    }
    
    List cards;
    cards = CardGetListForDrawPile(gameId,con);
    for(int i=0;i<cards.size;i++) {
        int cardid = ((Card*)ListGetVal(&cards,i)->ptr)->id;
        CardRemoveFromDraw(cardid,gameId,con);
        CardDelete(cardid,con);
    }
    
    cards = CardGetListForPlayedPile(gameId,con);
    for(int i=0;i<cards.size;i++) {
        int cardid = ((Card*)ListGetVal(&cards,i)->ptr)->id;
        CardRemoveFromPlayed(cardid,gameId,con);
        CardDelete(cardid,con);
    }
    
    List players = GetPlayersInGame(gameId,con);
    for(int j=0;j<players.size;j++) {
        int playerId = ((Player*)ListGetVal(&players,j)->ptr)->id;
        cards = CardGetListForPlayer(playerId,con);
        for(int i=0;i<cards.size;i++) {
            int cardid = ((Card*)ListGetVal(&cards,i)->ptr)->id;
            CardRemoveFromPlayer(cardid,playerId,con);
            CardDelete(cardid,con);
        }
    }
    res = ConnectionExec(con,StringFormatChar(arena,"update game set currentplayerindex = null,pluscounter=0 where gameid = %d",gameId));
    if(!(res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not update current player index\"}");
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