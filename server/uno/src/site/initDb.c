#include "../libs/server.c"
#include "../libs/utils/database.c"
#include "../libs/utils/cards.c"

String makeResponse(Connection con,struct Arena* arena) {
    String content;

    content = StringFrom("{\"ok\":true}",arena);

    FILE* fp = fopen("colors.json","r");
    if (fp) {
        ConnectionExec(con,StringFormatChar(arena,"delete from cardcolor"));
        String colorsstr = StringFrom("",arena);
        char c = fgetc(fp);
        while(!feof(fp)) {
            colorsstr = StringConcat(colorsstr,StringFromChar(c,arena),arena);
            c = fgetc(fp);
        }

        JsonElem* colorjson = JsonParse(colorsstr,arena);
        List* lst = (List*)colorjson->ptr;


        for(int i=0;i<lst->size;i++) {
            Hashmap* cardmap = ((JsonElem*)ListGetVal(lst,i)->ptr)->ptr;
            
            int id = *(int*)((JsonElem*)HashmapGet(cardmap,"id"))->ptr;
            String color = *(String*)((JsonElem*)HashmapGet(cardmap,"color"))->ptr;

            ConnectionExec(con,StringFormatChar(arena,"insert into cardcolor(cardcolorid,color) values(%d,'%S')",id,color));
        }
    }

    fp = fopen("types.json","r");
    if (fp) {
        ConnectionExec(con,StringFormatChar(arena,"delete from cardtype"));
        String typesstr = StringFrom("",arena);
        char c = fgetc(fp);
        while(!feof(fp)) {
            typesstr = StringConcat(typesstr,StringFromChar(c,arena),arena);
            c = fgetc(fp);
        }

        JsonElem* typejson = JsonParse(typesstr,arena);
        List* lst = (List*)typejson->ptr;

        for(int i=0;i<lst->size;i++) {
            Hashmap* typemap = ((JsonElem*)ListGetVal(lst,i)->ptr)->ptr;
            int id = *(int*)((JsonElem*)HashmapGet(typemap,"id"))->ptr;
            String desc = *(String*)((JsonElem*)HashmapGet(typemap,"desc"))->ptr;

            ConnectionExec(con,StringFormatChar(arena,"insert into cardtype(cardtypeid,carddesc) values(%d,'%S')",id,desc));
        }
    }

    fp = fopen("defaultdeck.json","r");
    if(fp) {
        ConnectionExec(con,StringFormatChar(arena,"delete from deck where deckid = 1"));
        ConnectionExec(con,StringFormatChar(arena,"insert into deck(deckid,deckname) values(1,\'default\')"));
        List cardlst = QueryResultToList(ConnectionSelect(con,StringFrom("select * from deckcard where deckid = 1",arena)),arena);
        for(int i=0;i<cardlst.size;i++) {
            Hashmap* cardmap = ListGetVal(&cardlst,i)->ptr;
            String cardid = *(String*)HashmapGet(cardmap,"cardid");
            ConnectionExec(con,StringFormatChar(arena,"delete from deckcard where cardid=%S ",cardid));
            ConnectionExec(con,StringFormatChar(arena,"delete from gamecard where cardid=%S ",cardid));
        }
        String deckstr = StringFrom("",arena);
        char c = fgetc(fp);
        while(!feof(fp)) {
            deckstr = StringConcat(deckstr,StringFromChar(c,arena),arena);
            c = fgetc(fp);
        }

        JsonElem* deckjson = JsonParse(deckstr,arena);
        List* lst = (List*)deckjson->ptr;
        for(int i=0;i<lst->size;i++) {
            Hashmap* obj = ((JsonElem*)ListGetVal(lst,i)->ptr)->ptr;
            
            String* desc = ((JsonElem*)HashmapGet(obj,"desc"))->ptr;
            QueryResult res = ConnectionSelect(con,StringFormatChar(arena,"select cardtypeid from cardtype where carddesc like \'%S\'",*desc));
            int* converr = ArenaAlloc(arena,sizeof(int));
            
            int typeid = StringToInt(*(String*)HashmapGet(QueryResultToMap(res,arena),"cardtypeid"),converr);
            
            List* values = ((JsonElem*)HashmapGet(obj,"values"))->ptr;
            
            

            List* colors = ((JsonElem*)HashmapGet(obj,"colors"))->ptr;

            for(int i=0;i<colors->size;i++) {
                int colorid = *(int*)((JsonElem*)ListGetVal(colors,i)->ptr)->ptr;
                for(int j=0;j<values->size;j++) {
                    List* valtuple = ((JsonElem*)ListGetVal(values,j)->ptr)->ptr;
                    int cardval = *(int*)((JsonElem*)ListGetVal(valtuple,0)->ptr)->ptr;
                    int cardamount = *(int*)((JsonElem*)ListGetVal(valtuple,1)->ptr)->ptr;
                    for(int k=0;k<cardamount;k++) {
                        Card card;
                        card.id = -1;
                        card.value = cardval;
                        card.colorId = colorid;
                        card.typeId = typeid;
                        CardInsert(&card,con);
                        CardAddToDeck(card.id,1,con);
                    }
                }

            }
        }
    }
    

    return content;
}

int main(int argc,char** argv) {
    ServerInitDefault();

    struct Arena arena = ArenaCreate(1024);

    Connection con = ConnectionNew("unodb","uno","root","pass4root","5432");

    ServerRespond(200,StringFrom("{\"Content-Type\":\"application/json\"}",&arena),makeResponse(con,&arena),&arena);

    ConnectionClose(con);
    ArenaDelete(&arena);
}