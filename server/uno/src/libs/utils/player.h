#include "database.c"
#include "json.c"

typedef struct {
    int id;
    String username;
    String userPwd;
    String* imagepath;
    int* joinedGameId;
    int* createdGameId;
    int* gameIndex;
    int* saidUno;
}Player;

Player PlayerFindById(int id, Connection con);

Player PlayerFromMap(Hashmap* playermap, struct Arena* arena);

List GetPlayersInGame(int gameID, Connection con);

QueryResult InsertPlayer(Player* player, Connection con);

QueryResult PlayerDelete(int id, Connection con);

QueryResult AddPlayerToGame(int playerid, int gameId, Connection con);

