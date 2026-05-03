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
    String* cardId = HashmapGet(&map,"cardId");
    err = cardId ? err : StringConcat(err,StringFrom("missing cardId in request ",arena),arena);
    if(!userId || !userPwd || !cardId) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from usercard where playerid = %S and cardid = %S",*userId,*cardId));
    if (!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no card with this id in player's hand\"}");
    }

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S and userpwd like '%S'",*userId,*userPwd));
    if (!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id\"}");
    }
    
    

    Hashmap* player = QueryResultToMap(res,arena);

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where gameid = %S",*(String*)HashmapGet(player,"joinedgameid")));
    
    Hashmap* game = QueryResultToMap(res,arena);

    int gameId = StringToInt(*(String*)HashmapGet(game,"gameid"),converr);

    int gameIndex = StringToInt(*(String*)HashmapGet(game,"currentplayerindex"),converr);

    

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where joinedgameid = %d order by gameindex asc",gameId));
    List players = QueryResultToList(res,arena);
    

    Hashmap* currentplayer = ListGetVal(&players,gameIndex)->ptr;
    
    int currentplayerid = StringToInt(*(String*)HashmapGet(currentplayer,"playerid"),converr);
    if (currentplayerid != StringToInt(*userId,converr)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"it isn't this player's turn\"}");
    }

    Card card = CardFindById(StringToInt(*cardId,converr),con);

    Card currentCard = CardFindById(StringToInt(
        *(String*)HashmapGet(
            QueryResultToMap(
                ConnectionSelect(con,StringFormatChar(arena,"select cardid from playedpilecard where gameid = %d order by cardindex desc",gameId)),
                arena
            ),
            "cardid"
        ),
        converr
    ),con);

    if(!(card.colorId == -1 || currentCard.colorId == -1 || currentCard.colorId == card.colorId || (currentCard.typeId == card.typeId && currentCard.value == card.value))) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"incorrect card color %d and %d %d %d %d %d\"}",currentCard.colorId,card.colorId,currentCard.typeId, card.typeId , currentCard.value ,card.value);
    }

    int skipid = StringToInt(
        *(String*)HashmapGet(
            QueryResultToMap(
                ConnectionSelect(con,StringFrom("select cardtypeid from cardtype where carddesc like 'skip'",arena)),
                arena
            ),
            "cardtypeid"
        ),
        converr
    );

    int reverseid = StringToInt(
        *(String*)HashmapGet(
            QueryResultToMap(
                ConnectionSelect(con,StringFrom("select cardtypeid from cardtype where carddesc like 'reverse'",arena)),
                arena
            ),
            "cardtypeid"
        ),
        converr
    );

    int wildid = StringToInt(
        *(String*)HashmapGet(
            QueryResultToMap(
                ConnectionSelect(con,StringFrom("select cardtypeid from cardtype where carddesc like 'wild'",arena)),
                arena
            ),
            "cardtypeid"
        ),
        converr
    );

    int plusid = StringToInt(
        *(String*)HashmapGet(
            QueryResultToMap(
                ConnectionSelect(con,StringFrom("select cardtypeid from cardtype where carddesc like 'plus'",arena)),
                arena
            ),
            "cardtypeid"
        ),
        converr
    );

    int pluswildid = StringToInt(
        *(String*)HashmapGet(
            QueryResultToMap(
                ConnectionSelect(con,StringFrom("select cardtypeid from cardtype where carddesc like 'pluswild'",arena)),
                arena
            ),
            "cardtypeid"
        ),
        converr
    );

    int k = 1;

    int isReversed = StringToInt(*(String*)HashmapGet(game,"isreversed"),converr);

    if (card.typeId == skipid) {
        k++;
    }

    if (card.typeId == reverseid) {
        isReversed = !isReversed;
    }

    k = isReversed?-k:k;

    gameIndex = (gameIndex+k) % players.size;
    gameIndex = gameIndex < 0 ? gameIndex+players.size : gameIndex;

    ConnectionExec(con,StringFormatChar(arena,"update game set isreversed = %d, currentplayerindex = %d where gameid = %d",isReversed,gameIndex,gameId));

    CardAddToPlayed(card.id,gameId,con);
    CardRemoveFromPlayer(card.id,StringToInt(*userId,converr),con);

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