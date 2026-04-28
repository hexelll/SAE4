#include "cards.h"

Card CardFindById(int id,Connection con) {
    struct Arena scratch = ArenaCreate(1024);
    QueryResult res = ConnectionSelect(con,StringFormatChar(&scratch,"select * from gamecard where cardid = %d",id));
    Hashmap* cardmap = QueryResultToMap(res,&con.arena);
    Card card = CardFromMap(*cardmap,con);
    ArenaDelete(&scratch);
    return card;
}

Card CardFromMap(Hashmap cardmap,Connection con) {
    struct Arena scratch = ArenaCreate(1024);
    Card card;
    int* converr = ArenaAlloc(&scratch,sizeof(int));
    card.id = StringToInt(*(String*)HashmapGet(&cardmap,"cardid"),converr);
    card.value = StringToInt(*(String*)HashmapGet(&cardmap,"cardvalue"),converr);
    card.colorId = StringToInt(*(String*)HashmapGet(&cardmap,"cardcolorid"),converr);
    QueryResult res = ConnectionSelect(con,StringFormatChar(&scratch,"select * from cardcolor where cardcolorid = %d",card.colorId));
    Hashmap* colormap = QueryResultToMap(res,&con.arena);
    card.colorHex = *(String*)HashmapGet(colormap,"color");
    card.typeId = StringToInt(*(String*)HashmapGet(&cardmap,"cardtypeid"),converr);
    res = ConnectionSelect(con,StringFormatChar(&scratch,"select * from cardtype where cardtypeid = %d",card.typeId));
    Hashmap* typemap = QueryResultToMap(res,&con.arena);
    card.typeDesc = *(String*)HashmapGet(typemap,"carddesc");
    ArenaDelete(&scratch);
    return card;
}

List CardGetListForPlayer(int id,Connection con) {
    struct Arena scratch = ArenaCreate(1024);
    QueryResult res = ConnectionSelect(con,StringFormatChar(&scratch,"select * from usercard where playerid = %d order by cardid",id));
    List cardTuples = QueryResultToList(res,&con.arena);
    List cards = ListNew(&con.arena);
    int* converr = ArenaAlloc(&scratch,sizeof(int));
    for(int i = 0;i<cardTuples.size;i++) {
        Hashmap cardmap = *(Hashmap*)ListGetVal(&cardTuples,i)->ptr;
        Card* card = ArenaAlloc(&con.arena,sizeof(Card));
        *card = CardFindById(StringToInt(*(String*)HashmapGet(&cardmap,"cardid"),converr),con);
        ListAppendVal(&cards,(ListValue){card});
    }
    ArenaDelete(&scratch);
    return cards;
}

List CardGetListForPlayedPile(int id,Connection con) {
    struct Arena scratch = ArenaCreate(1024);
    QueryResult res = ConnectionSelect(con,StringFormatChar(&scratch,"select * from playedpilecard where gameid = %d order by cardindex",id));
    List cardTuples = QueryResultToList(res,&con.arena);
    List cards = ListNew(&con.arena);
    int* converr = ArenaAlloc(&scratch,sizeof(int));
    for(int i = 0;i<cardTuples.size;i++) {
        Hashmap cardmap = *(Hashmap*)ListGetVal(&cardTuples,i)->ptr;
        Card* card = ArenaAlloc(&con.arena,sizeof(Card));
        *card = CardFindById(StringToInt(*(String*)HashmapGet(&cardmap,"cardid"),converr),con);
        ListAppendVal(&cards,(ListValue){card});
    }
    ArenaDelete(&scratch);
    return cards;
}

List CardGetListForDrawPile(int id,Connection con) {
    struct Arena scratch = ArenaCreate(1024);
    QueryResult res = ConnectionSelect(con,StringFormatChar(&scratch,"select * from drawpilecard where gameid = %d",id));
    List cardTuples = QueryResultToList(res,&con.arena);
    List cards = ListNew(&con.arena);
    int* converr = ArenaAlloc(&scratch,sizeof(int));
    for(int i = 0;i<cardTuples.size;i++) {
        Hashmap cardmap = *(Hashmap*)ListGetVal(&cardTuples,i)->ptr;
        Card* card = ArenaAlloc(&con.arena,sizeof(Card));
        *card = CardFindById(StringToInt(*(String*)HashmapGet(&cardmap,"cardid"),converr),con);
        ListAppendVal(&cards,(ListValue){card});
    }
    ArenaDelete(&scratch);
    return cards;
}

QueryResult CardInsert(Card* card,Connection con) {
    if (card->id >= 0)
        return ConnectionExec(con,StringFormatChar(&con.arena,"insert into gamecard(cardid,cardvalue,cardcolorid,cardtypeid) values(%d,%d,%d,%d)",card->id,card->value,card->colorId,card->typeId));
    QueryResult res = ConnectionSelect(con,StringFormatChar(&con.arena,"select max(cardid) as cardid from gamecard"));
    int* converr = ArenaAlloc(&con.arena,sizeof(int));
    String idstr = *(String*)HashmapGet(QueryResultToMap(res,&con.arena),"cardid");
    card->id = 0;
    if(idstr.size > 0)
        card->id = StringToInt(idstr,converr)+1;
    return CardInsert(card,con);
}

QueryResult CardDelete(int id,Connection con) {
    return ConnectionExec(con,StringFormatChar(&con.arena,"delete from gamecard where cardid = %d",id));
}

QueryResult CardAddToPlayer(int cardid,int playerid,Connection con) {
    return ConnectionExec(con,StringFormatChar(&con.arena,"insert into usercard(playerid,cardid) values(%d,%d)",playerid,cardid));
}

QueryResult CardRemoveFromPlayer(int cardid,int playerid,Connection con) {
    return ConnectionExec(con,StringFormatChar(&con.arena,"delete from usercard where cardid = %d and playerid = %d",cardid,playerid));
}

QueryResult CardAddToPlayed(int cardid,int gameid,Connection con) {
    QueryResult res = ConnectionSelect(con,StringFormatChar(&con.arena,"select max(cardindex) as cardindex from playedpilecard"));
    int* converr = ArenaAlloc(&con.arena,sizeof(int));
    String cardindexstr = *(String*)HashmapGet(QueryResultToMap(res,&con.arena),"cardindex");
    int cardindex = 0;
    if(cardindexstr.size > 0)
        cardindex = StringToInt(cardindexstr,converr)+1;
    return ConnectionExec(con,StringFormatChar(&con.arena,"insert into playedpilecard(gameid,cardid,cardindex) values(%d,%d,%d)",gameid,cardid,cardindex));
}

QueryResult CardRemoveFromPlayed(int cardid,int gameid,Connection con) {
    return ConnectionExec(con,StringFormatChar(&con.arena,"delete from playedpilecard where cardid = %d and gameid = %d",cardid,gameid));
}

QueryResult CardAddToDraw(int cardid,int gameid,Connection con) {
    return ConnectionExec(con,StringFormatChar(&con.arena,"insert into drawpilecard(gameid,cardid) values(%d,%d)",gameid,cardid));
}

QueryResult CardRemoveFromDraw(int cardid,int gameid,Connection con) {
    return ConnectionExec(con,StringFormatChar(&con.arena,"delete from drawpilecard where cardid = %d and gameid = %d",cardid,gameid));
}

QueryResult CardAddToDeck(int cardid,int deckid,Connection con) {
    return ConnectionExec(con,StringFormatChar(&con.arena,"insert into deckcard(deckid,cardid) values(%d,%d)",deckid,cardid));
}
