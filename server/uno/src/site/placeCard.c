#include <stdio.h>
#include "/myserver/libs/utils/database.c"
#include "/myserver/libs/server.c"

List getCardsForPlayer(String playerid,struct Arena* arena, Connection theconnection) {
    QueryResult resnb = ConnectionSelect(theconnection, StringFormatChar(arena,"select * from gamecard,usercard,player where gamecard.cardid = usercard.cardid and usercard.playerid = player.playerid and player.playerid = %S",playerid));
    return QueryResultToList(resnb,arena);
}

String makeResponse(struct Arena* arena,Hashmap map) {
    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");
    
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

    QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where playerid = %S and userpwd = '%S'",*userId,*userPwd));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }

    String query = StringFormatChar(arena, "Select * from gamecard where cardid = %S", *cardId);
    FILE* fp = fopen("./logfile.txt", "a");
    fprintf(fp, StringToChar(query, arena));
    fclose(fp);
    QueryResult res2 = ConnectionSelect(con , query);
    if(!(res2.count > 0 && res2.message.size == 0)){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"card does not exits at the curent time\"}");
    }

    res = ConnectionSelect(con, StringFormatChar(arena, "Select * from usercard where cardid = %S and playerid = %S", *cardId, *userId));
    if(!(res.count > 0 && res.message.size == 0)){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"player does not own provided card\"}");
    }

    List nbCards = getCardsForPlayer(*userId, arena, con);
    if (nbCards.size < 0){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"user has no cards\"}");
    }

    ConnectionClose(con);
    return StringFormatChar(arena,"{\"ok\":true}");


}

int main(int argc,char** argv) {
    ServerInitDefault();
    struct Arena sarena = ArenaCreate(1024);

    struct Arena* arena = &sarena;

    String response = makeResponse(arena,ServerParseRequest(argv,arena));

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",arena),response,arena);
    ArenaDelete(arena);
}