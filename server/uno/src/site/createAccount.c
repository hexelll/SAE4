#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/list.c"
#include "../libs/utils/player.c"
#include "/myserver/libs/utils/cards.c"



String makeResponse(struct Arena* arena,Hashmap map, Connection con) {

    String err = StringFrom("",arena);
    String* userName = HashmapGet(&map,"username");
    err = userName ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);

    if(!userName || !userPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }
    
    Player lePlayer;
    lePlayer.id = -1;
    lePlayer.username = *userName;
    lePlayer.userPwd = *userPwd;

    if(lePlayer.username.size > 10){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Username size exceeds character limit (10 characters)\",\"id\":\"%d\"}", -1);
    }
    for(int = 0; i<=lePlayer.username.size, i++){
        char lechar = lePlayer.username.text[i];
        if(lechar < 'a' || lechar > 'z' || lechar < 'A' || lechar > 'Z'){
            StringFormatChar(arena,"{\"ok\":false,\"error\":\"Please input only letters in your username\",\"id\":\"%d\"}", -1)
        }
    }
    QueryResult res = InsertPlayer(&lePlayer, con);
    if(!(res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Creation of account failed\",\"id\":\"%d\"}", -1);
    }else{
                QueryResult res = ConnectionSelect(con,StringFormatChar(&con.arena,"select max(playerid) from player"));
                int* converr = ArenaAlloc(&con.arena, sizeof(int));
                String idstr = *(String*)HashmapGet(QueryResultToMap(res,&con.arena),"max");
                int leID = StringToInt(idstr,converr);
                return StringFormatChar(arena,"{\"ok\":true,\"error\":\"\", \"id\": %d}", leID);
    }
    return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Query did not get executed\",\"id\":\"%d\"}", -1);
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