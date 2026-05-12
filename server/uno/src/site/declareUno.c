#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "/myserver/libs/utils/cards.c"


String makeResponse(struct Arena* arena,Connection con,Hashmap map) {
    String err = StringFrom("",arena);
    String* userId = HashmapGet(&map,"userId");
    int* converr = ArenaAlloc(arena,sizeof(int));
    int userIdnb = 0;
    err = userId ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    userIdnb = userId ? StringToInt(*userId, &userIdnb) : -1;
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);
    if(!userId || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }
    

    String query = StringFormatChar(arena,"select * from player where playerid = %d and userpwd = '%S'",userIdnb,*userPwd);
    QueryResult res = ConnectionSelect(con,query);
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }

    Hashmap* playermap = QueryResultToMap(res,arena);

    List nbCards = CardGetListForPlayer(userIdnb,con);
    if(nbCards.size == 0){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"couldn't retrieve number of card in the player's current hand\"}");
    }
    String gameIdStr = *(String*)HashmapGet(playermap,"joinedgameid");
    if (gameIdStr.size == 0) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"this user isn't in a game\"}");
    }
    int gameId = StringToInt(gameIdStr,converr);

    if(nbCards.size == 1){
        ConnectionExec(con,StringFormatChar(arena,"update player set saiduno = 1 where playerid = %d",userIdnb));
    }else {
        for(int i=0;i<2;i++) {
            List drawPile = CardGetListForDrawPile(gameId,con);
            int i = rand()%drawPile.size;
            i = i<0?i+drawPile.size:i;
            Card card = *(Card*)ListGetVal(&drawPile,i)->ptr;
            CardRemoveFromDraw(card.id,gameId,con);
            CardAddToPlayer(card.id,userIdnb,con);
        }
    }
    return StringFormatChar(arena,"{\"ok\":true}");
}

int main(int argc,char** argv) {
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(1024);

    struct Arena* arena = &sarena;
    if(argc < 4) {
        String response = StringFormatChar(arena,"{\"ok\":false,\"error\":\"missing request arguments\"}");
        ServerRespond(400,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
        ArenaDelete(arena);
        return 1;
    }

    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");
    String response = makeResponse(arena,con,ServerParseRequest(argv,arena));
    ConnectionClose(con);
    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
    ArenaDelete(arena);
}