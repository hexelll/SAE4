#include <stdio.h>
#include "/myserver/libs/server.c"
#include "/myserver/libs/utils/database.c"

List getCardsForPlayer(String playerid,Connection con,struct Arena* arena) {
    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from gamecard,usercard where gamecard.cardid = usercard.cardid and usercard.playerid = %S",playerid));
    if(res.count > 0 && res.message.size > 0) return QueryResultToList(res,arena);
    return ListNew(arena);
}

String makeResponse(struct Arena* arena,char** argv) {

    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");
    
    Hashmap map = ServerParseRequest(argv,arena);
    String err = StringFrom("",arena);
    String* userId = HashmapGet(&map,"userId");
    err = userId ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);
    if(!userId || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S",*userId));
    if (!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id\"}");
    }
    
    List currplayertuples = QueryResultToList(res,arena);
    Hashmap* user = ListGetVal(&currplayertuples,0)->ptr;
    String* gameId = HashmapGet(user,"joinedgameid");

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where joinedgameid = %S and not (playerid = %S)",*gameId,*userId));
    List playertuples = QueryResultToList(res,arena);
    
    Hashmap response = HashmapNew(sizeof(JsonElem),arena);
    List responsePlayers = ListNew(arena);
    
    
    
    int* converr = ArenaAlloc(arena,sizeof(int));
    for(int i=0;i<playertuples.size;i++) {
        Hashmap playermap = HashmapNew(sizeof(JsonElem),arena);
        Hashmap* player = ListGetVal(&playertuples,i)->ptr;
        String* playerId = (String*)HashmapGet(player,"playerid");
        int playerid = StringToInt(*playerId,converr);
        HashmapSetInt(&playermap,"playerId",playerid);
        String username = *(String*)HashmapGet(player,"username");
        HashmapSetString(&playermap,"username",username);
        List cards = getCardsForPlayer(*playerId,con,arena);
        HashmapSetInt(&playermap,"cardCount",cards.size);      
        ListAppendMap(&responsePlayers,playermap);
    }
    
    HashmapSetList(&response,"players",responsePlayers);
    List playercards = getCardsForPlayer(*userId,con,arena);
    List responseCards = ListNew(arena);
    Hashmap cardmap;
    //return StringFormatChar(arena,"{\"size\":%d}",playercards.size);
    //String s = StringFrom("",arena);
    for(int i=0;i<playercards.size;i++) {
        
        cardmap = HashmapNew(sizeof(JsonElem),arena);
        Hashmap* playercard = ListGetVal(&playercards,i)->ptr;
        HashmapSetInt(&cardmap,"cardId",StringToInt(*(String*)HashmapGet(playercard,"cardid"),converr));
        HashmapSetInt(&cardmap,"cardValue",StringToInt(*(String*)HashmapGet(playercard,"cardvalue"),converr));
        HashmapSetString(&cardmap,"cardColor",*(String*)HashmapGet(playercard,"cardcolor"));
        ListAppendMap(&responseCards,cardmap);
    }
    
    HashmapSetList(&response,"cards",responseCards);
    
    res = ConnectionSelect(con,StringFormatChar(arena,"select * from gamecard,playedpilecard where gamecard.cardid = playedpilecard.cardid and playedpilecard.gameid = %S order by playedpilecard.cardindex desc",*gameId));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no card in pile\"}");
    }
    Hashmap* currCard = QueryResultToList(res,arena).head.val.ptr;
    
    cardmap = HashmapNew(sizeof(JsonElem),arena);
    HashmapSetInt(&cardmap,"cardId",StringToInt(*(String*)HashmapGet(currCard,"cardid"),converr));
    HashmapSetInt(&cardmap,"cardValue",StringToInt(*(String*)HashmapGet(currCard,"cardvalue"),converr));
    HashmapSetString(&cardmap,"cardColor",*(String*)HashmapGet(currCard,"cardcolor"));
    HashmapSetMap(&response,"currentCard",cardmap);
    
    res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where gameid = %S",*gameId));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no game with this id\"}");
    }
    
    Hashmap* gameMap = QueryResultToList(res,arena).head.val.ptr;

    HashmapSetBool(&response,"isReserved",StringToInt(*(String*)HashmapGet(gameMap,"isreversed"),converr));
    HashmapSetInt(&response,"currentPlayerIndex",StringToInt(*(String*)HashmapGet(gameMap,"currentplayerindex"),converr));

    HashmapSetBool(&response,"ok",1);

    String jsonResponse = JsonFromHashmap(&response,arena);
    //String jsonResponse = StringFrom("{\"ok\":true}",arena);
    ConnectionClose(con);

    return jsonResponse;
}

int main(int argc,char** argv) {   
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(10*1024);

    struct Arena* arena = &sarena;

    String response = makeResponse(arena,argv);

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
    ArenaDelete(arena);

}