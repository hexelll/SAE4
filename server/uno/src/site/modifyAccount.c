#include <stdio.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"
#include "../libs/utils/list.c"
#include "../libs/utils/player.c"
#include "/myserver/libs/utils/cards.c"



String makeResponse(struct Arena* arena,Hashmap map, Connection con) {

    String err = StringFrom("",arena);
    String* userId = HashmapGet(&map,"userId");
    String* userPwd = HashmapGet(&map,"userPwd");
    err = userPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);
    err = userId ? err : StringConcat(err,StringFrom("missing userId in request ",arena),arena);
    String* newUserName = HashmapGet(&map,"newUserName");
    err = newUserName ? err : StringConcat(err,StringFrom("missing newUserName in request ",arena),arena);
    String* newUserPwd = HashmapGet(&map,"newUserPwd");
    err = newUserPwd ? err : StringConcat(err,StringFrom("missing userPwd in request ",arena),arena);

    if(!userPwd||!userId||!newUserName || !newUserPwd) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"%S\"}",err);
    }
    int* converr = ArenaAlloc(arena, sizeof(int));
    Player lePlayer;
    lePlayer.id = StringToInt(*userId,converr);
    lePlayer.username = *newUserName;
    lePlayer.userPwd = *newUserPwd;

    if(lePlayer.username.size > 10){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Username size exceeds character limit (10 characters)\"}");
    }
    for(int i = 0; i<lePlayer.username.size; i++){
        char lechar = lePlayer.username.text[i];
        if((lechar < 'a' || lechar > 'z') && (lechar < 'A' || lechar > 'Z')){
            return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Please input only letters in your username\"}");
        }
    }

    if(lePlayer.userPwd.size > 10){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"User password size exceeds character limit (10 characters)\"}");
    }
    for(int i = 0; i<lePlayer.userPwd.size; i++){
        char lechar = lePlayer.userPwd.text[i];
        if((lechar < 'a' || lechar > 'z') && (lechar < 'A' || lechar > 'Z')){
            return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Please input only letters in your password\"}");
        }
    }

    QueryResult checkAlready = ConnectionSelect(con ,StringFormatChar(arena, "Select * from player where playerid= %d and userpwd like '%S'", lePlayer.id, *userPwd));
    if(!(checkAlready.count > 0 && checkAlready.message.size == 0)){
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"No user with this id and password\"}");
    }

    QueryResult res = ConnectionExec(con ,StringFormatChar(arena,"Update player set username = '%S', userpwd = '%S' where playerid = %d", lePlayer.username, lePlayer.userPwd, lePlayer.id));
    if(!(res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"Update of account failed\"}");
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