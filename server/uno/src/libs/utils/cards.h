#include "database.c"
#include "json.c"

typedef struct {
    int id;
    int value;
    int colorId;
    String colorHex;
    int typeId;
    String typeDesc;
}Card;

Card CardFindById(int id,Connection con);

Card CardFromMap(Hashmap cardmap,Connection con);

List CardGetListForPlayer(int id,Connection con);

List CardGetListForPlayedPile(int id,Connection con);

List CardGetListForDrawPile(int id,Connection con);

QueryResult CardInsert(Card* card,Connection con);

QueryResult CardDelete(int id,Connection con);

QueryResult CardAddToPlayer(int cardid,int playerid,Connection con);

QueryResult CardRemoveFromPlayer(int cardid,int playerid,Connection con);

QueryResult CardAddToPlayed(int cardid,int gameid,Connection con);

QueryResult CardRemoveFromPlayed(int cardid,int gameid,Connection con);

QueryResult CardAddToDraw(int cardid,int gameid,Connection con);

QueryResult CardRemoveFromDraw(int cardid,int gameid,Connection con);

QueryResult CardAddToDeck(int cardid,int deckid,Connection con);