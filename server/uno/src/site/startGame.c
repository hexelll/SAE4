#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../libs/utils/database.c"
#include "../libs/server.c"

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
    /*if(playerindex->size > 0) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"this game is already started %S\"}",*playerindex);
    }*/

    res = ConnectionSelect(con,StringFormatChar(arena,"select * from player where joinedgameid = %S",gameId));
    if(!(res.count > 0 && res.message.size == 0)) {
        return StringFormatChar(arena,"{\"ok\":false,\"error\":\"no players in this game\"}");
    }

    List players = QueryResultToList(res,arena);

    ConnectionExec(con,StringFormatChar(arena,"update game set currentplayerindex = 0,isReversed = 0 where gameid=%S",gameId));

    String defaultCardsString = StringFrom("",arena);
    FILE* fp = fopen("./defaultcards.json","r");

    char c = fgetc(fp);
    while(!feof(fp)) {
        defaultCardsString = StringConcat(defaultCardsString,StringFromChar(c,arena),arena);
        c=fgetc(fp);
    }
    fclose(fp);
    
    JsonElem* defaultCardsJson = JsonParse(defaultCardsString,arena);
    List* defaultCards = (List*)defaultCardsJson->ptr;

    //return JsonToString(defaultCardsJson,arena);

    int ncards = 7;

    srand(time(NULL));

    
    for(int i=0;i<players.size;i++) {
        Hashmap* player = ListGetVal(&players,i)->ptr;
        String playerId = *(String*)HashmapGet(player,"playerid");
        ConnectionExec(con,StringFormatChar(arena,"delete from usercard where playerid=%S",playerId));
        
        for(int j=0;j<ncards;j++) {
            int itype = rand()%defaultCards->size;
            itype = itype < 0 ? itype + defaultCards->size : itype;
            Hashmap* cardMap = ((JsonElem*)ListGetVal(defaultCards,itype)->ptr)->ptr;
            String cardDesc = *(String*)(((JsonElem*)HashmapGet(cardMap,"cardDesc"))->ptr);
            List* values = ((JsonElem*)HashmapGet(cardMap,"values"))->ptr;
            List* colors = ((JsonElem*)HashmapGet(cardMap,"colors"))->ptr;
            int ival = rand()%values->size;
            ival = ival < 0 ? ival + values->size : ival;
            int icol = rand()%colors->size;
            icol = icol < 0 ? icol + colors->size : icol;
            int value = *(int*)((JsonElem*)ListGetVal(values,ival)->ptr)->ptr;
            int colorid = *(int*)((JsonElem*)ListGetVal(colors,icol)->ptr)->ptr;
            res = ConnectionSelect(con,StringFrom("select max(cardid) as newid from gamecard",arena));
            List cardTuple = QueryResultToList(res,arena);
            int cardid = StringToInt(*(String*)HashmapGet((Hashmap*)ListGetVal(&cardTuple,0)->ptr,"newid"),converr)+1;
            ConnectionExec(con,StringFormatChar(arena,"insert into gamecard(cardid,cardvalue,cardcolorid,cardtypeid) values(%d,%d,%d,(select cardtypeid from cardtype where carddesc like '%S'))",cardid,value,colorid,cardDesc));
            ConnectionExec(con,StringFormatChar(arena,"insert into usercard(cardid,playerid) values(%d,%S)",cardid,playerId));
        }
        int itype = rand()%defaultCards->size;
        itype = itype < 0 ? itype + defaultCards->size : itype;
        Hashmap* cardMap = ((JsonElem*)ListGetVal(defaultCards,itype)->ptr)->ptr;
        String cardDesc = *(String*)(((JsonElem*)HashmapGet(cardMap,"cardDesc"))->ptr);
        List* values = ((JsonElem*)HashmapGet(cardMap,"values"))->ptr;
        List* colors = ((JsonElem*)HashmapGet(cardMap,"colors"))->ptr;
        int ival = rand()%values->size;
        ival = ival < 0 ? ival + values->size : ival;
        int icol = rand()%colors->size;
        icol = icol < 0 ? icol + colors->size : icol;
        int value = *(int*)((JsonElem*)ListGetVal(values,ival)->ptr)->ptr;
        int colorid = *(int*)((JsonElem*)ListGetVal(colors,icol)->ptr)->ptr;
        res = ConnectionSelect(con,StringFrom("select max(cardid) as newid from gamecard",arena));
        List cardTuple = QueryResultToList(res,arena);
        int cardid = StringToInt(*(String*)HashmapGet((Hashmap*)ListGetVal(&cardTuple,0)->ptr,"newid"),converr)+1;
        ConnectionExec(con,StringFormatChar(arena,"delete from playedpilecard where gameid = %S",gameId));
        ConnectionExec(con,StringFormatChar(arena,"insert into gamecard(cardid,cardvalue,cardcolorid,cardtypeid) values(%d,%d,%d,(select cardtypeid from cardtype where carddesc like '%S'))",cardid,value,colorid,cardDesc));
        ConnectionExec(con,StringFormatChar(arena,"insert into playedpilecard(gameid,cardid,cardindex) values(%S,%d,0)",gameId,cardid));
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