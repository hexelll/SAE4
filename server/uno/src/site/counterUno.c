#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/list.c"
#include "../libs/utils/cards.c"
#include "../libs/utils/player.c"

List getAllPlayersByGameId(int gameId , struct Arena* arena, Connection theconnection){
    QueryResult resAllPlayers = ConnectionSelect(theconnection, StringFormatChar(arena, "select * from player where joinedgameid = %d", gameId));
    return QueryResultToList(resAllPlayers, arena);
}

int getPlayerGameId(String playerid, struct Arena* arena, Connection theconnection){
    QueryResult resPlayers = ConnectionSelect(theconnection, StringFormatChar(arena, "select * from player where playerid =%S", playerid));
    List tuples = QueryResultToList(resPlayers,arena);
    Hashmap* user = ListGetVal(&tuples,0)->ptr;
    String* gameId = HashmapGet(user,"joinedgameid");
    int gameIdNumber = -1;
    return StringToInt(*gameId , &gameIdNumber);
}

String makeResponse(struct Arena* arena,Hashmap map, Connection con) {

    String err = StringFrom("",arena);
    String* userId = HashmapGet(&map,"userId");
    int userIdnb = 0;
    err = userId ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    userIdnb = userId ? StringToInt(*userId, &userIdnb) : -1;
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);

    if(!userId || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    int gameId = getPlayerGameId(*userId, arena, con);
    if(gameId <= 0){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not find game\"}");
    }
    
    
    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %d and userpwd = '%S'",userIdnb,*userPwd));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }
    
    List players = GetPlayersInGame(gameId, con);
    List theresults = ListNew(arena);
    if(players.size > 0){
        for (int i=0; i < players.size; i++){
            
            Player* user = ListGetVal(&players,i)->ptr;
            int playerId = user->id;
            int playerUno = *user->saidUno;
            List cards = CardGetListForPlayer(playerId, con);
            if (cards.size == 1){
                Hashmap* leplayer = ArenaAlloc(arena,sizeof(Hashmap));
                *leplayer = HashmapNew(sizeof(JsonElem), arena);
                HashmapSetInt(leplayer, "playerId", playerId);
                Hashmap* result= ArenaAlloc(arena,sizeof(Hashmap));
                *result = HashmapNew(sizeof(JsonElem), arena);
                
                if(playerUno != 1){
                    HashmapSetString(result,"ok",StringFormatChar(arena,"true"));
                }else{
                    HashmapSetString(result,"ok",StringFormatChar(arena,"false"));
                    HashmapSetString(result,"error", StringFrom("this user already said uno", arena));
                    for(int i=0;i<2;i++) {
                        List drawPile = CardGetListForDrawPile(gameId,con);
                        if (drawPile.size == 0) {
                            List playedPile = CardGetListForPlayedPile(gameId,con);
                            while (playedPile.size > 1) {
                                int j = rand()%(playedPile.size-1);
                                j = j < 0 ? j+(playedPile.size-1):j;
                                Card* c = ListGetVal(&playedPile,j)->ptr;
                                CardAddToDraw(c->id,gameId,con);
                                CardRemoveFromPlayed(c->id,gameId,con);
                                playedPile = CardGetListForPlayedPile(gameId,con);
                            }
                        }
                        drawPile = CardGetListForDrawPile(gameId,con);
                        int i = rand()%drawPile.size;
                        i = i<0?i+drawPile.size:i;
                        Card card = *(Card*)ListGetVal(&drawPile,i)->ptr;
                        CardRemoveFromDraw(card.id,gameId,con);
                        CardAddToPlayer(card.id,userIdnb,con);
                    }
                }
                HashmapSetMap(leplayer, "result", *result);
                JsonElem* leElem = ArenaAlloc(arena,sizeof(JsonElem));
                leElem->ptr = leplayer;
                leElem->type = OBJECT;
                ListAppendJsonElem(&theresults, leElem);
            }            
        }
        if(theresults.size > 0){
            return JsonFromList(&theresults, arena);
        }
        
    }else{
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no players in that game\"}");
    }
    return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no players with 1 card left\"}");
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