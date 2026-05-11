// INCLUDE WIFI
#include "secrets.h"
#include "WiFiS3.h"
// JSON
#include "ArduinoJson.h"
// SCREEN
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

/*
---------- DATA DECLARATION  ------------------------------------------------------------------------------------------
*/
// Server connection data
const char server[] = "10.97.197.104";//"10.106.120.104";//10.106.120.33
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
bool LAST_RESPONSE_ABORTED = false;
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

// requests data
long timeGetGameState = millis();

// Other players data
struct Player {
  int id = -1;
  String username;
  int nbCards;
};
Player playerList[4] = {};
int nbPlayers = 0; // last player = nbPlayers - 1

// Local player's cards
struct Card {
  int id = -1;
  int typeId = -1;
  int colorId = -1;
  int value = -1;
  String colorHex;
  String typeDescription;
};
Card myCards[30] = {};
int myNbCards = 0; // last card = myNbCards - 1

// Game data
int gameId = -1;
String gamePassword = "";
Card lastPlayedCard = {
    .id = -1,
    .typeId = -1,
    .colorId = -1,
    .value = -1,
    .colorHex = "888888",
    .typeDescription = String("")
  };
int currentPlayerIndex;
bool isReversed = false;

// user data
int myId = 2;
String myPassword = "enorme";

// Menu data
enum MenuStates {NONE,TITLE_SCREEN,PAUSE,IN_GAME,CHOOSE_GAME,SETINGS,CONTROLS};
enum MenuStates wantedMenuState = TITLE_SCREEN; // default menu (when restarting)
enum MenuStates currentMenuState = NONE;
short positionInHand = 0;
short positionInMenu = 0;

// Screen
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);



/*
---------- MAIN  ------------------------------------------------------------------------------------------
*/
void setup() {

  // I/O PINS
  pinMode(A0,INPUT);        // joystick x
  pinMode(A1,INPUT);        // joystick y
  pinMode(2,INPUT_PULLUP);  // menu
  pinMode(3,INPUT_PULLUP);  // ok
  pinMode(4,INPUT_PULLUP);  // no
  pinMode(5,INPUT_PULLUP);  // uno
  pinMode(7,INPUT_PULLUP);  // joystick pressed

  // Initialize serial and wait for port to open:
  Serial.begin(500000);
  while (!Serial);

  // start screen
  tft.begin(); 
  tft.setTextColor(ILI9341_WHITE);
  tft.setRotation(3);
  changeInterface();
  
  // connect to wifi
  initWifi(SECRET_SSID,SECRET_PASS);

  // TEST
  //queueNewRequest("GET","/getGameState.c?userId=1&userPwd=enorme","", callback_getGameState );
  //queueNewRequest("GET","/playCard.c?userId=1&userPwd=jaja&cardId=8","", callback_errorHandler );
  //queueNewRequest("GET","/getGameState.c?userId=1&userPwd=jaja","", callback_getGameState );

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

  showStartText();
}

void loop(){
  // process http requests/responses
  processResponses();

  // inputs
  handleInputs();

  // change Interface if necessary
  changeInterface();
  // update visual effects (blinking,...)
  updateActiveEffects();

  // call getGameState every 1.5s
  if (currentMenuState == IN_GAME || currentMenuState == PAUSE){
    long currentTime = millis();
    if (currentTime - timeGetGameState > 2000){
      timeGetGameState = millis();
      queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_getGameState );
    }
  }

}




