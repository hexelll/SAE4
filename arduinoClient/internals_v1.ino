// includes
#include "secrets.h"
#include <ArduinoJson.h>
#include "WiFiS3.h"

// server IP
const char server[] = "10.106.120.104";
const int port = 9999;

// String-ish structure
template <int N>
struct Text {
  int length = 0;
  char body[N];
};
// 
template <int N>
struct Response {
  int code;
  Text<N> data;
};

enum requestType { NONE , TEST };

// necessary global declarations
WiFiClient client;
JsonDocument responseData;
bool RESPONSE_ONGOING = false;
requestType lastSentRequest = NONE;

int temps = 0;

void processResponse(){
  if (!RESPONSE_ONGOING && !responseData.isNull()){

    switch (lastSentRequest){
      case NONE:
        break;
      case TEST: 
        const char* test = responseData["param"][0]["test"];
        Serial.println();
        Serial.println(test);
        Serial.println();
        Serial.println( String( millis()-temps ) );
        break;
    }
    
  }
}

void setup() {

  pinMode(12,INPUT_PULLUP);

  // Initialize serial and wait for port to open:
  Serial.begin(500000);
  while (!Serial);
  
  // connect to wifi
  initWifi(SECRET_SSID,SECRET_PASS);

  client.setTimeout(60000);
  
}

void loop(){

  if ( !digitalRead(12) ) {
    //sendHTTPRequest(String("GET"),String("/"),String(""));
    if (sendHTTPRequest(String("POST"),String("/testJson.c"),String("{\"test\"=\"valeur2test\"")) ){
      Serial.println("[OK] request sent");
      lastSentRequest = TEST;
      temps = millis();
    }else{
      Serial.println("[ERROR] error in server connection");
    }
  }

  // receive the HTTP response and loads it into responseData && responseCode
  receiveHTTPResponse( true );
  
  // processes the response in response to take actions
  processResponse();

  //delay(100);
  //Serial.println("A");
}




