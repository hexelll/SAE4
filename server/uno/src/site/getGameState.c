#include <stdio.h>
#include "../libs/server.c"
#include "../libs/utils/database.c"
#include "../libs/utils/cards.c"

String makeResponse(struct Arena* arena,Connection con,char** argv) {    
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
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }
    
    List currplayertuples = QueryResultToList(res,arena);
    Hashmap* user = ListGetVal(&currplayertuples,0)->ptr;
    String* gameId = HashmapGet(user,"joinedgameid");

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where joinedgameid = %S and not (playerid = %S) order by gameindex asc",*gameId,*userId));
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
        List cards = CardGetListForPlayer(playerid,con);
        HashmapSetInt(&playermap,"cardCount",cards.size);      
        ListAppendMap(&responsePlayers,playermap);
    }
    
    HashmapSetList(&response,"players",responsePlayers);
    List playercards = CardGetListForPlayer(StringToInt(*userId,converr),con);

    List responseCards = ListNew(arena);
    Hashmap cardmap;
    for(int i=0;i<playercards.size;i++) {
        cardmap = HashmapNew(sizeof(JsonElem),arena);
        Card* playercard = ListGetVal(&playercards,i)->ptr;
        HashmapSetInt(&cardmap,"cardId",playercard->id);
        HashmapSetInt(&cardmap,"cardValue",playercard->value);
        HashmapSetInt(&cardmap,"cardColorId",playercard->colorId);
        HashmapSetString(&cardmap,"cardColorHex",playercard->colorHex);
        HashmapSetInt(&cardmap,"cardTypeId",playercard->typeId);
        HashmapSetString(&cardmap,"cardTypeDesc",playercard->typeDesc);
        ListAppendMap(&responseCards,cardmap);
    }
    
    HashmapSetList(&response,"cards",responseCards);
    
    List cards = CardGetListForPlayedPile(StringToInt(*gameId,converr),con);

    Card* currCard = ListGetVal(&cards,cards.size-1)->ptr;
    cardmap = HashmapNew(sizeof(JsonElem),arena);
    HashmapSetInt(&cardmap,"cardId",currCard->id);
    HashmapSetInt(&cardmap,"cardValue",currCard->value);
    HashmapSetInt(&cardmap,"cardColorId",currCard->colorId);
    HashmapSetString(&cardmap,"cardColorHex",currCard->colorHex);
    HashmapSetInt(&cardmap,"cardTypeId",currCard->typeId);
    HashmapSetString(&cardmap,"cardTypeDesc",currCard->typeDesc);
    HashmapSetMap(&response,"currentCard",cardmap);
    
    res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where gameid = %S",*gameId));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no game with this id\"}");
    }
    
    Hashmap* gameMap = QueryResultToList(res,arena).head.val.ptr;

    HashmapSetBool(&response,"isReversed",StringToInt(*(String*)HashmapGet(gameMap,"isreversed"),converr));
    HashmapSetInt(&response,"currentPlayerIndex",StringToInt(*(String*)HashmapGet(gameMap,"currentplayerindex"),converr));

    HashmapSetBool(&response,"ok",1);

    String jsonResponse = JsonFromHashmap(&response,arena);

    return jsonResponse;
}

int main(int argc,char** argv) {   
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(10*1024);

    struct Arena* arena = &sarena;

    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");

    String response = makeResponse(arena,con,argv);

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
    ConnectionClose(con);
    ArenaDelete(arena);

}