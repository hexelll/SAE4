#include <stdio.h>
#include "/myserver/libs/server.c"
#include "/myserver/libs/utils/database.c"

int main(int argc,char** argv) {   
    ServerInitDefault();
    struct Arena* sarena = ArenaCreate(1024);
    
    Connexion con = ConnectionNew("unodb","uno","root","pass4root","5432");
    Hashmap map = ServerParseRequest(argv,&sarena);
    String* error = StringFrom("",&sarena); 
    String* userID = HashmapGet(&map "userId");
    error = userID ? error : StringConcat(error, StringFrom("error in the user ID recovery\n",&sarena));
    String* passU = HashmapGet(&map,"userPwd");
    error = passU ? error : StringConcat(error, StringFrom("error in the user password recovery",&sarena));
    QueryResut gamecode = ConnectionSelect(con "Select gameid from game join player on creatorid = createdgameid");
    // ^ a modifier
    error = gamecode ? error : StringConcat(error, StringFrom("error in the game id recovery",&sarena));

    if (gamecode || userID || passU){
        
    }



    ServerRespond(200,StringFrom("{\"Content-Type\":\"text/html\"}",&sarena),text,&sarena);
    ConnectionClose(con.con);
    ArenaDelete(&sarena);

}