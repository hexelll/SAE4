#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/list.c"
#include "../libs/utils/player.c"
#include "/myserver/libs/utils/cards.c"



String makeResponse(struct Arena* arena,Hashmap map, Connection con) {

    String err = StringFrom("",arena);
    String* userid = HashmapGet(&map,"userId");
    err = userid ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);

    if(!userid || !userPwd ) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    int* converr = ArenaAlloc(&con.arena, sizeof(int));
    int leUserId = StringToInt(*userid, converr);
    String query = StringFormatChar(arena,"select * from player where playerid = %d and userpwd = '%S'",leUserId,*userPwd);
    QueryResult resCheck1 = ConnectionSelect(con,query);
    if(!(resCheck1.count > 0 && resCheck1.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }
    
    Hashmap* player = QueryResultToMap(resCheck1,arena);

    String gameId = *(String*)HashmapGet(player,"createdgameid");
    if(gameId.size == 0) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"this user isn't a game owner\"}");
    }
    int leGameId = StringToInt(gameId, converr);

    String querychk2 = StringFormatChar(arena,"select currentplayerindex from game where gameid = '%d'",leGameId);
    QueryResult resCheck2 = ConnectionSelect(con,querychk2);
    if(!(resCheck2.count > 0 && resCheck2.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not retrieve player index\"}");
    }

    String* index = (String*)HashmapGet(QueryResultToMap(resCheck2,&con.arena),"currentplayerindex");
    if (index && index->size != 0){
       return StringFormatChar(arena,"{\"ok\":false,\"error\":\"cannot delete a running game\"}");
    }
    

    QueryResult delgame = ConnectionExec(con, StringFormatChar(arena, "Delete from game where gameid = %d", leGameId));
    if(!(delgame.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}", delgame.message);
    }

    QueryResult reset = ConnectionExec(con, StringFormatChar(arena, "update player set joinedgameid = NULL, gameindex = NULL where joinedgameId = %d", leGameId));
    if(!(reset.message.size == 0)){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not update joinedgameId -> %S\"}", reset.message);
    }

    QueryResult resetCreator = ConnectionExec(con, StringFormatChar(arena, "update player set createdgameid = NULL where playerid = %d", leUserId));
    if(!(resetCreator.message.size == 0)){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not update players created game id -> %S\"}", resetCreator.message);
    }

    return StringFormatChar(arena,"{\"ok\":true}");
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