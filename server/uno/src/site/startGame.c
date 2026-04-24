#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/cards.c"

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

    Hashmap* creator = ListGetVal(&creatorTuple,0)->ptr;

    String gameId = *(String*)HashmapGet(creator,"createdgameid");

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where gameid = %S",gameId));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no game was created by this user\"}");
    }
    List gameTuple = QueryResultToList(res,arena);

    Hashmap* game = ListGetVal(&gameTuple,0)->ptr;
    String* playerindex =(String*)HashmapGet(game,"currentplayerindex");
    if(playerindex->size > 0) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"this game is already started\"}");
    }

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where joinedgameid = %S",gameId));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no players in this game\"}");
    }

    List players = QueryResultToList(res,arena);

    ConnectionExec(con,StringFormatChar(arena,"update game set currentplayerindex = 0,isReversed = 0 where gameid=%S",gameId));

    List cardlst = QueryResultToList(ConnectionSelect(con,StringFrom("select * from deckcard where deckid = 1",arena)),arena);
    
    srand(time(NULL));

    for(int i=0;i<players.size;i++){
        Hashmap* playermap = ListGetVal(&players,i)->ptr;
        int playerid = StringToInt(*(String*)HashmapGet(playermap,"playerid"),converr);
        ConnectionExec(con,StringFormatChar(arena,"delete from usercard where playerid = %d",playerid));
        for(int j=0;j<7;j++) {
            int k = abs(rand())%cardlst.size;
            int cardid = StringToInt(*(String*)HashmapGet(ListGetVal(&cardlst,k)->ptr,"cardid"),converr);
            CardAddToPlayer(cardid,playerid,con);
            ListRemoveNode(&cardlst,k);
        }
    }
    int gameIdInt = StringToInt(gameId,converr);
    ConnectionExec(con,StringFormatChar(arena,"delete from playedpilecard where gameid = %S",gameId));
    int k = abs(rand())%cardlst.size;
    int cardid = StringToInt(*(String*)HashmapGet(ListGetVal(&cardlst,k)->ptr,"cardid"),converr);
    CardAddToPlayed(cardid,gameIdInt,con);
    ListRemoveNode(&cardlst,k);

    ConnectionExec(con,StringFormatChar(arena,"delete from drawpilecard where gameid = %S",gameId));
    for(int i=0;i<cardlst.size;i++) {
        int cardid = StringToInt(*(String*)HashmapGet(ListGetVal(&cardlst,i)->ptr,"cardid"),converr);
        CardAddToDraw(cardid,gameIdInt,con);
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