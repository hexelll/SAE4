
// begin wifi connection
void initWifi(char ssid[],char pass[]){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("[ERROR] Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  
  // check wifi firmware
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("[WARNING] Please upgrade the firmware");
  }
  
  // attempt to connect to WiFi network:
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    // wait a few seconds for connection:
    Serial.println("[WARNING] WIFI connection failed, retrying in 2s");
    delay(2000);
  }
  Serial.println("[OK] WIFI connected");
}

// Send an HTTP Request with given values, returns connection success/failure
bool sendHTTPRequest(String method, String ressourcePath, String dataSent){
  if (client.connect(server, port)) {
    String request = method + String(" ") + ressourcePath + String(" HTTP/1.1\nHost: ") + String(server) + String("\nConnection: close\n\n") + dataSent + String("\0\0");
    client.print(request); 
    return true;
  }else{
    return false;
  }
}

bool contentLengthFound(int c) {
    const char marker[] = "Content-Length:";  // search string
    static int pos = 0;  // correct characters received so far
    if (c == -1) return false;  // shortcut for a common case
    if (c == marker[pos]) {
        pos++;                      // count a correct character
        if (marker[pos] == '\0') {  // we got the complete string
            pos = 0;                // reset for next time
            return true;            // report success
        }
    } else if (c == marker[0]) {
        pos = 1;        // we have the initial character again
    } else {
        pos = 0;        // we received a wrong character
    }
    return false;
}

bool endOfHeaderFound(int c) {
    const char marker[] = "\n\r\n";  // search string
    static int pos = 0;  // correct characters received so far
    if (c == -1) return false;  // shortcut for a common case
    if (c == marker[pos]) {
        pos++;                      // count a correct character
        if (marker[pos] == '\0') {  // we got the complete string
            pos = 0;                // reset for next time
            return true;            // report success
        }
    } else if (c == marker[0]) {
        pos = 1;        // we have the initial character again
    } else {
        pos = 0;        // we received a wrong character
    }
    return false;
}

void receiveHTTPResponse(bool debug){
  static bool RESPONSE_HEADER_ONGOING = true;
  static bool RESPONSE_CONTENT_LENGTH_ONGOING = false;
  static Text<5> contentLengthBuffer;
  static int contentLength = 0;
  static Response<2048> response;

  while (client.available()) {
    RESPONSE_ONGOING = true;

    char c = client.read();


    if (debug) {
      Serial.print(c);
      //Serial.print(" | ");
      //Serial.println( (int)c );
    }

    if(!RESPONSE_HEADER_ONGOING){
      response.data.body[response.data.length] = c;
      response.data.length ++;
      contentLength--;
    }

    if(RESPONSE_CONTENT_LENGTH_ONGOING && RESPONSE_HEADER_ONGOING){
      if(c== '\n'){
        RESPONSE_CONTENT_LENGTH_ONGOING = false;
        contentLength = atoi(contentLengthBuffer.body);
        memset(contentLengthBuffer.body, 0, sizeof(contentLengthBuffer.body));
        contentLengthBuffer.length = 0;
      }else{
        contentLengthBuffer.body[contentLengthBuffer.length] = c;
        contentLengthBuffer.length++;
      }
    }

    if( contentLengthFound(c) && RESPONSE_HEADER_ONGOING){
      RESPONSE_CONTENT_LENGTH_ONGOING = true;
    }

    if ( endOfHeaderFound(c) && RESPONSE_HEADER_ONGOING) {
      RESPONSE_HEADER_ONGOING = false;
    }

    if(contentLength <= 0 && !RESPONSE_HEADER_ONGOING ){
      RESPONSE_ONGOING = false;
      RESPONSE_HEADER_ONGOING = true;
      return;
    }

  } 

  if (!RESPONSE_ONGOING){
    deserializeJson(responseData, response.data.body);
    memset(response.data.body, 0, response.data.length);
    response.data.length = 0;
  }
}