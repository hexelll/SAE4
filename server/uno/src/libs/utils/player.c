#include "player.h"

Player PlayerFindById(int id, Connection con){
    struct Arena sarena = ArenaCreate(1024);
    QueryResult res = ConnectionSelect(con,  StringFormatChar(&sarena, "Select * from player where playerid = %d", id));
    Hashmap* playermap = QueryResultToMap(res,&con.arena);
    Player player = PlayerFromMap(*playermap,con);
    ArenaDelete(&sarena);
    return player;
};

Player PlayerFromMap(Hashmap playermap, Connection con){
    struct Arena scratch = ArenaCreate(1024);
    Player player;
    int* converr = ArenaAlloc(&scratch, sizeof(int));
    player.id = StringToInt(*(String*)HashmapGet(&playermap,"playerid"), converr);
    player.username= *(String*)HashmapGet(&playermap,"username");
    player.userPwd = *(String*)HashmapGet(&playermap,"userpwd");
    player.imagepath = (String*)HashmapGet(&playermap,"imagepath");
    int jgameID = StringToInt(*(String*)HashmapGet(&playermap,"joinedgameid"), converr);
    int cgameID = StringToInt(*(String*)HashmapGet(&playermap,"createdgameid"), converr);
    int gameIndex = StringToInt(*(String*)HashmapGet(&playermap,"gameindex"),converr);
    int saidUno = StringToInt(*(String*)HashmapGet(&playermap,"saiduno"),converr);
    player.joinedGameId = &jgameID;
    player.createdGameId = &cgameID;
    player.gameIndex = &gameIndex;
    player.saidUno = &saidUno;
    ArenaDelete(&scratch);
    return player;
};

List GetAlPlayers(Connection con){
    struct Arena scratch = ArenaCreate(1024);
    QueryResult res = ConnectionSelect(con,  StringFromChar(*"select * from player",&scratch));
    List playersTuples = QueryResultToList(res,&con.arena);
    List players = ListNew(&con.arena);
    int* converr = ArenaAlloc(&scratch, sizeof(int));
    for(int i = 0; i <playersTuples.size;i++){
        Hashmap playermap = *(Hashmap*)ListGetVal(&playersTuples,i)->ptr;
        Player* player = ArenaAlloc(&con.arena,sizeof(Player));
        *player = PlayerFindById(StringToInt(*(String*)HashmapGet(&playermap,"cardid"),converr),con);
        ListAppendVal(&players,(ListValue){player});
    }
    ArenaDelete(&scratch);
    return players;
}

List GetPlayersInGame(int gameID, Connection con){
    struct Arena scratch = ArenaCreate(1024);
    QueryResult res = ConnectionSelect(con, StringFormatChar(&scratch, "select * from player join game on player.joinedgameid = game.gameid where game.gameid = %d", gameID));
    List playersTuples = QueryResultToList(res, &con.arena);
    List players = ListNew(&con.arena);
    int* converr = ArenaAlloc(&scratch, sizeof(int));
    for(int i = 0; i<playersTuples.size;i++){
        Hashmap playermap = *(Hashmap*)ListGetVal(&playersTuples,i)->ptr;
        Player* player = ArenaAlloc(&con.arena,sizeof(Player));
        *player = PlayerFindById(StringToInt(*(String*)HashmapGet(&playermap,"playerid"),converr),con);
        ListAppendVal(&players, (ListValue){player});
    }
    ArenaDelete(&scratch);
    return players;
}

QueryResult InsertPlayer(Player* player, Connection con){
    if(player->id >= 0){
        return ConnectionExec(con,StringFormatChar(&con.arena,"insert into player(playerid,username,userpwd) values(%d,'%S','%S')",player->id,player->username,player->userPwd));
    }
    QueryResult res = ConnectionSelect(con,StringFormatChar(&con.arena,"select max(playerid) from player"));
    int* converr = ArenaAlloc(&con.arena, sizeof(int));
    String idstr = *(String*)HashmapGet(QueryResultToMap(res,&con.arena),"max");
    player->id = 0;
    if(idstr.size > 0)
        player->id = StringToInt(idstr,converr)+1;
    return InsertPlayer(player, con);
};

QueryResult PlayerDelete(int id, Connection con){
    return ConnectionExec(con,StringFormatChar(&con.arena, "delete from player where playerid = %d", id));
};

QueryResult AddPlayerToGame(int playerid, int gameId, Connection con){
    return ConnectionExec(con, StringFormatChar(&con.arena, "update player set joinedgameid = %d where playerid = %d", gameId, playerid));
};