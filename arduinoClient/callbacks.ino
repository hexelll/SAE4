/*
---------- REQUEST CALLBACKS ------------------------------------------------------------------------------------------
*/
  
// debug callback
void callback_dumpResponse(Request* requestSent,JsonDocument* responseData){
  Serial.println(F("\n\n-------------\n>>> Request sent :"));
  Serial.println((*requestSent).method + F(" ") + (*requestSent).path + F(" HTTP/1.1\nHost: ") + String(server) + F("\nConnection: close\n\n") + (*requestSent).data);
  Serial.println(F("> Response data received :"));
  serializeJson(*responseData, Serial);
}

// handles "ok" and "error" fields in request
void callback_errorHandler(Request* requestSent,JsonDocument* responseData){
  requestWorked = (*responseData)["ok"];
  if ( (*responseData).containsKey("error")){
    requestError = (*responseData)["error"].as<String>();
    Serial.println(F("\n[WARNING] Request returned not ok with error :"));
    Serial.println(requestError);
  }
}

// getGameState callback
void callback_getGameState(Request* requestSent,JsonDocument* responseData){
  callback_errorHandler(requestSent,responseData);
  if (requestWorked){
    
    // update last played card
    Card newLastCard = newCardFromJson( (*responseData)["currentCard"] );
    bool lastCard_CHANGED = newLastCard.id != lastPlayedCard.id;
    lastPlayedCard = newLastCard;

    // update current player index
    bool currentPlayerIndex_CHANGED = false;
    if (currentPlayerIndex != (*responseData)["currentPlayerIndex"]){
      currentPlayerIndex_CHANGED = true;
      currentPlayerIndex = (*responseData)["currentPlayerIndex"];
    }

    // update is reversed
    isReversed = (*responseData)["isReversed"];

    // update my cards
    bool cards_CHANGED = false;
    myNbCards = 0;
    JsonArray cardsArray = (*responseData)["cards"];
    for (JsonObject cardData : cardsArray ){
      if (myNbCards >= 30) {
        Serial.println("Too many cards!");
        break;
      }
      Card newCard = newCardFromJson(cardData);
      if (myCards[myNbCards].id != newCard.id){
        cards_CHANGED = true;
      }
      myCards[myNbCards] = newCard;
      myNbCards++;
    }

    // update players
    bool players_CHANGED = false;
    nbPlayers = 0;
    JsonArray playersArray = (*responseData)["players"];
    for (JsonObject playerData : playersArray ){
      if (nbPlayers >= 4) {
        break;
      }
      Player newPlayer = newPlayerFromJson(playerData);
      
      if (playerList[nbPlayers].id != newPlayer.id){
        players_CHANGED = true;
      }
      playerList[nbPlayers] = newPlayer;
      nbPlayers++;
    }

    // update display if data changed
    if (lastCard_CHANGED){
      updatePile();
    }
    if (cards_CHANGED){
      updateCards();
    }
    if (players_CHANGED || currentPlayerIndex_CHANGED){
      updatePlayers();
    }
  }
}