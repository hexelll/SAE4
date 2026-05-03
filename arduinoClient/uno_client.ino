#include "secrets.h"
#include <ArduinoJson.h>
#include "WiFiS3.h"


/*
---------- DATA DECLARATION  ------------------------------------------------------------------------------------------
*/
// Server connection data
const char server[] = "172.31.78.33";//"10.106.120.104";//10.106.120.33
const int port = 42069; // 42069

// Request queue
struct Request {
  String method;
  String path;
  String data;
  void (*callback)(Request*,JsonDocument*);
};
Request requestQueue[30];
int requestQueueSize = 0;
int requestQueueIndex = 0;
Request lastSentRequest = {
  .method = String("NONE"),
  .path = String("/"),
  .data = String(""),
  .callback = [](Request* requestSent,JsonDocument* responseData){
    Serial.println(F("NOT SUPPOSED TO BE CALLED"));
  }
};

// Necessary global declarations
WiFiClient client;

// request responses errors
bool requestWorked = true;
String requestError = ""; // error received by request

// Other players data
struct Player {
  int id = -1;
  String username;
  int nbCards;
};
Player playerList[10] = {};
int nbPlayers = 0;

// Local player's cards
struct Card {
  int id = -1;
  int typeId = -1;
  int colorId = -1;
  int value = -1;
  String colorHex;
  String typeDescription;
};
Card myCards[20] = {};
int myNbCards = 0; // last card = nbCards - 1

// Game data
int gameId = -1;
String gamePassword = "";
Card lastPlayedCard;
int currentPlayerIndex;
bool isReversed = false;

// Menu data



/*
---------- UTILS ------------------------------------------------------------------------------------------
*/
Card newCardFromJson(JsonObject cardData){
  return {
    .id = cardData["cardId"].as<int>(),
    .typeId = cardData["cardTypeId"].as<int>(),
    .colorId = cardData["cardColorId"].as<int>(),
    .value = cardData["cardColorId"].as<int>(),
    .colorHex = cardData["cardColorHex"].as<String>(),
    .typeDescription = cardData["cardId"].as<String>()
  };
}
Player newPlayerFromJson(JsonObject cardData){
  return {
    .id = cardData["playerId"].as<int>(),
    .username = cardData["username"].as<String>(),
    .nbCards = cardData["cardCount"].as<int>()
  };
}


/*
---------- REQUEST CALLBACKS ------------------------------------------------------------------------------------------
*/
namespace Callback{
  
  // debug callback
  void dumpResponse(Request* requestSent,JsonDocument* responseData){
    Serial.println(F("\n\n-------------\n>>> Request sent :"));
    Serial.println((*requestSent).method + F(" ") + (*requestSent).path + F(" HTTP/1.1\nHost: ") + String(server) + F("\nConnection: close\n\n") + (*requestSent).data);
    Serial.println(F("> Response data received :"));
    serializeJson(*responseData, Serial);
  }

  // handles "ok" and "error" fields in request
  void errorHandler(Request* requestSent,JsonDocument* responseData){
    requestWorked = (*responseData)["ok"];
    if ( (*responseData).containsKey("error")){
      requestError = (*responseData)["error"].as<String>();
      Serial.println(F("\n[WARNING] Request returned not ok with error :"));
      Serial.println(requestError);
    }
  }

  // getGameState callback
  void getGameState(Request* requestSent,JsonDocument* responseData){
    errorHandler(requestSent,responseData);
    if (requestWorked){
      lastPlayedCard = newCardFromJson( (*responseData)["currentCard"] );

      currentPlayerIndex = (*responseData)["currentPlayerIndex"];

      isReversed = (*responseData)["isReversed"];

      myNbCards = 0;
      JsonArray cardsArray = (*responseData)["cards"];
      for (JsonObject cardData : cardsArray ){
        myCards[myNbCards] = newCardFromJson(cardData);
        Serial.println( myCards[myNbCards].id );
        myNbCards++;
      }

      nbPlayers = 0;
      JsonArray playersArray = (*responseData)["players"];
      for (JsonObject playerData : playersArray ){
        playerList[nbPlayers] = newPlayerFromJson(playerData);
        nbPlayers++;
      }

    }
  }

}


/*
---------- MAIN  ------------------------------------------------------------------------------------------
*/
void setup() {

  pinMode(12,INPUT_PULLUP); // use pin 12 as button input

  // Initialize serial and wait for port to open:
  Serial.begin(500000);
  while (!Serial);
  
  // connect to wifi
  initWifi(SECRET_SSID,SECRET_PASS);

  // TEST
  queueNewRequest("GET","/getGameState.c?userId=1&userPwd=jaja","", Callback::getGameState );
  queueNewRequest("GET","/playCard.c?userId=1&userPwd=jaja&cardId=8","", Callback::errorHandler );
  queueNewRequest("GET","/getGameState.c?userId=1&userPwd=jaja","", Callback::getGameState );

  /* LIST OF WORKING REQUESTS
  GET GAME STATE
  queueNewRequest("GET","/getGameState.c?userId=1&userPwd=jaja","", Callback::getGameState );
  DECLARE UNO
  queueNewRequest("GET","/declareUno.c?userId=1&userPwd=jaja","", Callback::errorHandler );
  DECLARE COUNTER UNO
  queueNewRequest("GET","/counterUno.c?userId=1&userPwd=jaja","", Callback::errorHandler );
  PLAY CARD
  queueNewRequest("GET","/playCard.c?userId=1&userPwd=jaja&cardId=8","", Callback::errorHandler );
  DRAW CARD
  
  JOIN GAME
  queueNewRequest("GET","/joinGame.c?userId=1&userPwd=jaja&gameCode=123","", Callback::errorHandler );
  */
}

void loop(){
  
  processResponses();

  //update screen

  /*
  if ( !digitalRead(12) ){
    queueNewRequest("GET","/getGameState.c?userId=2&userPwd=enorme","", Callback::dumpResponse );
  }
  */

}




