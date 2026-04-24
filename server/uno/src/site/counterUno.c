#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/list.c"

List getCardsForPlayer(String playerid,struct Arena* arena, Connection theconnection) {
    QueryResult resnb = ConnectionSelect(theconnection, StringFormatChar(arena,"select * from gamecard,usercard,player where gamecard.cardid = usercard.cardid and usercard.playerid = player.playerid and player.playerid = %S",playerid));
    return QueryResultToList(resnb,arena);
}

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

String makeResponse(struct Arena* arena,Hashmap map) {
    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");

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

    List players = getAllPlayersByGameId(gameId, arena, con);
    for (int i=0; i < players.size; i++){
            Hashmap* user = ListGetVal(&players,i)->ptr;
            String* playerId = HashmapGet(user,"playerid");
            List cards = getCardsForPlayer(*playerId, arena, con);
            if (cards.size == 1){
                return StringFormatChar(arena,"{\"ok\":true}");
            }
    }


    ConnectionClose(con);
    return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no players with 1 card left\"}");
}

int main(int argc,char** argv) {
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(1024);

    struct Arena* arena = &sarena;

    String response = makeResponse(arena,ServerParseRequest(argv,arena));

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
    ArenaDelete(arena);
}