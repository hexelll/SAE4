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
    String* gameId = HashmapGet(&map,"gameId");
    err = gameId ? err : StringConcat(err,StringFrom("missing gameId in request ",arena),arena);

    if(!userid || !userPwd || !gameId) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }

    int* converr = ArenaAlloc(&con.arena, sizeof(int));
    int leUserId = StringToInt(*userid, converr);
    int leGameId = StringToInt(*gameId, converr);
    String query = StringFormatChar(arena,"select * from player where playerid = %d and userpwd = '%S'",leUserId,*userPwd);
    QueryResult resCheck1 = ConnectionSelect(con,query);
    if(!(resCheck1.count > 0 && resCheck1.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }


    String querychk1 = StringFormatChar(arena,"select * from game where creatorid = %d and gameid = '%d'",leUserId,leGameId);
    QueryResult resCheck = ConnectionSelect(con,querychk1);
    if(!(resCheck.count > 0 && resCheck.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"User is not the owner of this game\"}");
    }


    String querychk2 = StringFormatChar(arena,"select currentplayerindex from game where gameid = '%d'",leGameId);
    QueryResult resCheck2 = ConnectionSelect(con,querychk2);
    if(!(resCheck2.count > 0 && resCheck2.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"could not retrieve player index\"}");
    }else{
         String* index = (String*)HashmapGet(QueryResultToMap(resCheck2,&con.arena),"currentplayerindex");
         if (index && (*index).size != 0){
            return StringFormatChar(arena,"{\"ok\":false,\"error\":\"cannot delete a running game\"}");
         }
    }



    QueryResult del = ConnectionExec(con, StringFormatChar(arena, "Delete from game where gameid = %d", leGameId));
    if(!(del.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}", del.message);
    }else{
        return StringFormatChar(arena,"{\"ok\":true,\"error\":\"\"}");
    }
    
    return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Query did not get executed\"}");
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