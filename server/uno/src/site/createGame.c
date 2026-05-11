#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include <time.h>

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

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S and userpwd = '%S'",*userId,*userPwd));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }


    QueryResult checkGameAlreadycreated = ConnectionSelect(con , StringFormatChar(arena, "select * from game where creatorid = %S",*userId));
    if(!(checkGameAlreadycreated.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}", checkGameAlreadycreated.message);
    }else{
        List playersTuples = QueryResultToList(checkGameAlreadycreated,&con.arena);
        if(playersTuples.size > 0){
            return StringFormatChar(arena,"{\"ok\":false,\"error\":\"user already createtd a game\"}");
        }
    }

    res = ConnectionSelect(con,StringFormatChar(arena,"select max(gameid) as id from game"));
    int id = 0;
    if(res.count > 0 && res.message.size == 0) {
        id = StringToInt(*(String*)HashmapGet(QueryResultToMap(res,arena),"id"),converr)+1;
    }
    srand(time(NULL));
    int ok = 0;
    String gameCode = StringNew(ArenaAlloc(arena,sizeof(char)*6),6);
    while(!ok) {
        for(int i=0;i<6;i++) {
            int n = rand()%10;
            n = n<0?n+10:n;
            gameCode.text[i] = 48+n;
        }
        res = ConnectionSelect(con,StringFormatChar(arena,"select * from game where code like '%S'",gameCode));
        ok = res.count == 0 && res.message.size == 0;
    }

    ConnectionExec(con,StringFormatChar(arena,"insert into game(gameid,code,deckid,creatorid,currentplayerindex,isreversed) values(%d,'%S',1,%S,NULL,0)",id,gameCode,*userId));

    ConnectionExec(con,StringFormatChar(arena,"update player set createdgameid = %d where playerid = %S",id,*userId));

    return StringFormatChar(arena,"{\"ok\":true, \"code\":\"%S\"}", gameCode);
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