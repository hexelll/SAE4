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
    if(!userId || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S and userpwd like '%S'",*userId,*userPwd));
    if (!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id\"}");
    }
    

    List players
    Hashmap* player = QueryResultToMap(res,arena);
    if(player->size > 0){
            res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where gameid = %S",*(String*)HashmapGet(player,"joinedgameid")));
            Hashmap* game = QueryResultToMap(res,arena);
            if(game->size){
                int gameId = StringToInt(*(String*)HashmapGet(game,"gameid"),converr);
                int gameIndex = StringToInt(*(String*)HashmapGet(game,"currentplayerindex"),converr);
                res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where joinedgameid = %d order by gameIndex asc",gameId));
                players = QueryResultToList(res,arena);
            }else{
                return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not convert game to hashmap\"}");
            }

    }else{
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not convert player to hashmap\"}");
    }

    Hashmap* currentplayer = ListGetVal(&players,gameIndex)->ptr;
    if(currentplayer){
            int currentplayerid = StringToInt(*(String*)HashmapGet(currentplayer,"playerid"),converr);
    }else{
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not find the curretn player\"}");
    }

    if (currentplayerid != StringToInt(*userId,converr)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"it isn't this player's turn\"}");
    }

    List drawcards = CardGetListForDrawPile(gameId,con);
    srand(time(NULL));

    if(drawcards.size > 0){
        int i = rand()%drawcards.size;
        i = i < 0 ? i+res.count : i;
    }else{
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not get pile of cards\"}");
    }

    Card* card = ListGetVal(&drawcards,i)->ptr;
    if(card){
       int cardid = card->id;
    }else{
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not get drawed card\"}");
    }
    int playerid = StringToInt(*userId,converr);

    CardAddToPlayer(cardid,playerid,con);
    CardRemoveFromDraw(cardid,gameId,con);

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