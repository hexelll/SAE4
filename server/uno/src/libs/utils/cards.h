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

List CardListForPlayer(int id,Connection con);

List CardListForPlayedPile(int id,Connection con);

List CardListForDrawPile(int id,Connection con);

QueryResult CardInsert(Card* card,Connection con);

QueryResult CardDelete(int id,Connection con);

Card CardFromMap(Hashmap cardmap,Connection con);