#include "../libs/utils/database.c"
#include "../libs/utils/cards.c"
#include "../libs/server.c"
#include <time.h>

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

    Card card = CardFindById(StringToInt(*cardId,converr),con);

    List playedLst = CardGetListForPlayedPile(gameId,con);

    Card currentCard = *(Card*)ListGetVal(&playedLst,playedLst.size-1)->ptr;

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

    String pluscounterstr = *(String*)HashmapGet(game,"pluscounter");
    int pluscounter = 0;
    if(pluscounterstr.size > 0) {
        pluscounter = StringToInt(pluscounterstr,converr);
    }

    if(!(
        pluscounter == 0 && 
        (card.colorId == -1 || currentCard.colorId == -1 || currentCard.colorId == card.colorId) || 
        (currentCard.typeId == card.typeId && currentCard.value == card.value) || 
        (card.typeId == plusid && currentCard.typeId == pluswildid || card.typeId == pluswildid && currentCard.typeId == plusid)
    )) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"incorrect card color\"}");
    }

    int k = 1;

    int isReversed = StringToInt(*(String*)HashmapGet(game,"isreversed"),converr);

    if (card.typeId == skipid) {
        k++;
    }

    if (card.typeId == reverseid) {
        isReversed = !isReversed;
    }

    

    if (card.typeId == plusid || card.typeId == pluswildid) {
        pluscounter += card.value;
    }

    k = isReversed?-k:k;

    if (pluscounter > 0) {
        int hasPlus = 0;
        int nextplayerindex = (gameIndex + k)%players.size;
        nextplayerindex += nextplayerindex < 0 ? players.size : 0;
        Hashmap* nextplayer = ListGetVal(&players,nextplayerindex)->ptr;
        int nextplayerid = StringToInt(*(String*)HashmapGet(nextplayer,"playerid"),converr);
        List cards = CardGetListForPlayer(nextplayerid,con);
        for(int i=0;i<cards.size;i++) {
            Card card = *(Card*)ListGetVal(&cards,i)->ptr;
            if(card.typeId == plusid || card.typeId == pluswildid) {
                hasPlus = 1;
                break;
            }
        }
        FILE* fp = fopen("logPlay.txt","w");
        fprintf(fp,StringToChar(StringFormatChar(arena,"\n pluscounter: %d",pluscounter),arena));
        fprintf(fp,StringToChar(StringFormatChar(arena,"\n k: %d",k),arena));
        fprintf(fp,StringToChar(StringFormatChar(arena,"\n nextplayerid: %d",nextplayerid),arena));
        if(!hasPlus) {
            
            fprintf(fp,"\n has plus");
            srand(time(NULL));
            for(int i=0;i<pluscounter;i++) {
                List drawPile = CardGetListForDrawPile(gameId,con);
                int i = rand()%drawPile.size;
                i = i<0?i+drawPile.size:i;
                Card card = *(Card*)ListGetVal(&drawPile,i)->ptr;
                CardRemoveFromDraw(card.id,gameId,con);
                CardAddToPlayer(card.id,nextplayerid,con);
            }
            k = k<0 ? k-1 : k+1;
            fprintf(fp,StringToChar(StringFormatChar(arena,"\n k: %d",k),arena));
            pluscounter = 0;
        }else {
            fprintf(fp,"fjreiuhjknbgct,h;ecfrbt,dvn gbjfhkejr");
        }
        

        fclose(fp);
    }

    gameIndex = (gameIndex+k) % players.size;
    gameIndex = gameIndex < 0 ? gameIndex+players.size : gameIndex;

    ConnectionExec(con,StringFormatChar(arena,"update game set isreversed = %d, currentplayerindex = %d, pluscounter = %d where gameid = %d",isReversed,gameIndex,pluscounter,gameId));
    int playerid = StringToInt(*userId,converr);
    CardAddToPlayed(card.id,gameId,con);
    CardRemoveFromPlayer(card.id,playerid,con);
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