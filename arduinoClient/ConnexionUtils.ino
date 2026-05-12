/*
---------- Wifi  ------------------------------------------------------------------------------------------
*/
// begin wifi connection
void initWifi(char ssid[],char pass[]){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println(F("[ERROR] Communication with WiFi module failed!"));
    // don't continue
    while (true);
  }
  
  // check wifi firmware
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println(F("[WARNING] Please upgrade the firmware"));
  }
  
  // attempt to connect to WiFi network:
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    // wait a few seconds for connection:
    Serial.println(F("[WARNING] WIFI connection failed, retrying in 2s"));
    delay(1000);
  }
  Serial.println(F("[OK] WIFI connected"));

  client.setTimeout(60000);
}


/*
---------- SENDING HTTP REQUESTS  ------------------------------------------------------------------------------------------
*/
// Sends given http Request, returns connection success/failure
bool sendHTTPRequest(Request requestToSend){
  if (client.connect(server, port)) {
    String request = requestToSend.method + F(" ") + requestToSend.path + F(" HTTP/1.1\nHost: ") + String(server) + F("\nConnection: close\r\n\r\n") + requestToSend.data ;
    client.print(request); 
    return true;
  }else{
    Serial.println(F("[ERROR] Could not connect to server"));
    return false;
  }
}


/*
---------- RECEIVING HTTP REQUESTS  ------------------------------------------------------------------------------------------
*/
// returns true if "Content-Length:" as been found in the last given chars
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

// returns true if "\n\r\n" (=end of header) as been found in the last given chars
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

// reads received chars, keeps response buffer, parses necessary info, converts to JsonDocument when the response is finished
JsonDocument receiveHTTPResponse(){
  // flags
  static bool RESPONSE_HEADER_ONGOING = true;
  static bool RESPONSE_ONGOING = false;
  static bool RESPONSE_CONTENT_LENGTH_ONGOING = false;
  // buffers
  static char contentLengthBuffer[10];
  static int contentLengthBufferLength = 0;
  static char responseBuffer[6000];
  static int responseBufferLength = 0;
  static bool RESPONSE_ABORTED = false;
  static int contentLength = 0;
  
  while (client.available()) {
    RESPONSE_ONGOING = true;

    char c = client.read();

    //Serial.print(c);
    //Serial.print(" | ");
    //Serial.println( (int)c );

    if (responseBufferLength >= sizeof(responseBuffer) - 1) {
      Serial.println(F("[ERROR] Response too large, request aborted"));
      LAST_RESPONSE_ABORTED = true;
      RESPONSE_ONGOING = false;
      RESPONSE_HEADER_ONGOING = true;
      RESPONSE_CONTENT_LENGTH_ONGOING = false;
      contentLength = 0;
      memset(contentLengthBuffer, 0, sizeof(contentLengthBuffer));
      contentLengthBufferLength = 0;
      memset(responseBuffer, 0, responseBufferLength);
      responseBufferLength = 0;
      // fully discard socket data
      while (client.available()) {
          client.read();
      }
      client.stop();
      break;
    }

    if(!RESPONSE_HEADER_ONGOING){
      responseBuffer[responseBufferLength] = c;
      responseBufferLength ++;
      contentLength--;
    }

    if(RESPONSE_CONTENT_LENGTH_ONGOING && RESPONSE_HEADER_ONGOING){
      if(c== '\n'){
        RESPONSE_CONTENT_LENGTH_ONGOING = false;
        contentLength = atoi(contentLengthBuffer);
        memset(contentLengthBuffer, 0, sizeof(contentLengthBuffer));
        contentLengthBufferLength = 0;
      }else{
        contentLengthBuffer[contentLengthBufferLength] = c;
        contentLengthBufferLength++;
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
      RESPONSE_CONTENT_LENGTH_ONGOING = false;

      contentLengthBufferLength = 0;
      contentLength = 0;
      break;
    }

  } 

  JsonDocument responseData;

  if (RESPONSE_ABORTED) {
      RESPONSE_ABORTED = false;
      return responseData; // null document
  }
  if (!RESPONSE_ONGOING && responseBufferLength > 0) {
      DeserializationError err = deserializeJson(responseData, responseBuffer);
      if (err) {
          Serial.print(F("[ERROR] JSON parse failed: "));
          Serial.println(err.c_str());
      }
      memset(responseBuffer, 0, responseBufferLength);
      responseBufferLength = 0;
  }

  return responseData;
}

/*
---------- REQUEST QUEUE MANAGEMENT  ------------------------------------------------------------------------------------------
*/
// adds given request to the queue, return false if queue is full
bool queueRequest(Request requestToSend){
  if ( requestQueueSize < 30 ){
    requestQueue[(requestQueueIndex+requestQueueSize)%30] = requestToSend;
    requestQueueSize++;
    return true;
  }else{
    Serial.println(F("[ERROR] Queue full : request dropped"));
    return false;
  }
}

// queueRequest wraper that builds new Request from easy to use args
bool queueNewRequest(char* method, String path, char* data, void (*callback)(Request*,JsonDocument*)){
  return queueRequest({
    .method = String(method),
    .path = path,
    .data = String(data),
    .callback = callback
  });
}

// util to handle retrieval of queue requests
Request popRequest(){
  int index = requestQueueIndex;
  requestQueueIndex++;
  requestQueueSize--;
  requestQueueIndex %= 30;
  return requestQueue[index];
}

// processes incoming http responses & sends next requests in queue when possible
void processResponses(){
  static bool IS_WAITING_FOR_RESPONSE = false;

  JsonDocument responseDataDoc = receiveHTTPResponse();
  if (LAST_RESPONSE_ABORTED) {
    LAST_RESPONSE_ABORTED = false;
    IS_WAITING_FOR_RESPONSE = false;
    return;
  }
  if ( !responseDataDoc.isNull() && IS_WAITING_FOR_RESPONSE){
    
    // last request response received
    lastSentRequest.callback(&lastSentRequest,&responseDataDoc);
  
    if (requestQueueSize > 0){
      lastSentRequest = popRequest();
      sendHTTPRequest(lastSentRequest);
      IS_WAITING_FOR_RESPONSE = true;
    }else{
      IS_WAITING_FOR_RESPONSE = false;
    }
  }
  if ( ! IS_WAITING_FOR_RESPONSE && requestQueueSize > 0){
    lastSentRequest = popRequest();
    sendHTTPRequest(lastSentRequest);
    IS_WAITING_FOR_RESPONSE = true;
  }
  
}
