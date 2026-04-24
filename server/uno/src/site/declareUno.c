#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"

List getCardsForPlayer(String playerid,struct Arena* arena, Connection theconnection) {
    QueryResult resnb = ConnectionSelect(theconnection, StringFormatChar(arena,"select * from gamecard,usercard,player where gamecard.cardid = usercard.cardid and usercard.playerid = player.playerid and player.playerid = %S",playerid));
    return QueryResultToList(resnb,arena);
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
    

    String query = StringFormatChar(arena,"select * from player where playerid = %d and userpwd = '%S'",userIdnb,*userPwd);
    QueryResult res = ConnectionSelect(con,query);
    FILE* fp = fopen("./logfile.txt","a");
    fprintf(fp, StringToChar(res.message, arena));
    fclose(fp);
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no user with this id and password\"}");
    }

    List nbCards = getCardsForPlayer(*userId, arena, con);
    if(!(nbCards.size > 0)){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"couldn't retrieve number of card in the player's current hand\"}");
    }

    if(nbCards.size > 1){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"player does not have 1 card left in hand\"}");
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