/*
---------- REQUEST CALLBACKS ------------------------------------------------------------------------------------------
*/
  
// debug callback
void callback_dumpResponse(Request* requestSent,JsonDocument* responseData){
  Serial.println(F("\n\n-------------\n>>> Request sent :"));
  Serial.println((*requestSent).method + F(" ") + (*requestSent).path + F(" HTTP/1.1\nHost: ") + String(server) + F("\nConnection: close\n\n"));
  Serial.println(F("> Response data received :"));
  serializeJson(*responseData, Serial);
}

// handles "ok" and "error" fields in request
void callback_errorHandler(Request* requestSent,JsonDocument* responseData){
  requestWorked = (*responseData)["ok"];
  if ( (*responseData).containsKey("error")){
    //requestError = (*responseData)["error"].as<String>();
    Serial.println(F("\n[WARNING] Request returned not ok with error :"));
    Serial.println((*responseData)["error"].as<String>());
  }
}

// handles create game 
void callback_createGame(Request* requestSent,JsonDocument* responseData){
  callback_errorHandler(requestSent,responseData);
  if (requestWorked){
    gameCode = (*responseData)["code"].as<String>();
    updateGameCodeDisplay();
  }
}

void callback_joinGame(Request* requestSent,JsonDocument* responseData){
  callback_errorHandler(requestSent,responseData);
  if (requestWorked){
    wantedMenuState = PAUSE;
    eraseJoinError();
  }else{
    displayJoinError();
  }
}

void callback_deleteGame(Request* requestSent,JsonDocument* responseData){
  callback_errorHandler(requestSent,responseData);
  if (requestWorked){
    wantedMenuState = TITLE_SCREEN;
  }
}

void callback_redirectToGame(Request* requestSent,JsonDocument* responseData){
  callback_errorHandler(requestSent,responseData);
  if (requestWorked){
    if (isStarted){
      wantedMenuState = IN_GAME;
    }else{
      wantedMenuState = PAUSE;
    }
  }
}

// getGameState callback
void callback_getGameState(Request* requestSent, JsonDocument* responseData){
  callback_errorHandler(requestSent, responseData);
  if (requestWorked) {

    JsonObject root = responseData->as<JsonObject>();

    bool nbCards_CHANGED = false;
    bool cards_CHANGED = false;
    bool players_CHANGED = false;
    bool lastCard_CHANGED = false;
    bool currentPlayerIndex_CHANGED = false;
    bool gameCode_CHANGED = false;
    bool isStarted_CHANGED = false;

    // Last played card
    Card newLastCard;
    newPointerCardFromJson(root["currentCard"], &newLastCard);
    if (newLastCard.id != lastPlayedCard.id) {
        lastPlayedCard = newLastCard;
        lastCard_CHANGED = true;
    }

    // Current player index
    int newCurrentPlayerIndex = root["currentPlayerIndex"];
    if (currentPlayerIndex != newCurrentPlayerIndex) {
        currentPlayerIndex = newCurrentPlayerIndex;
        currentPlayerIndex_CHANGED = true;
    }

    // Simple values
    isReversed = root["isReversed"];
    creatorId = root["creatorId"];

    // Game code
    String newGameCode = root["gameCode"].as<String>();
    if ( ! newGameCode.equals(gameCode) ) {
        gameCode = newGameCode;
        gameCode_CHANGED = true;
    }

    // Started state
    bool newIsStarted = root["isStarted"];
    if (!isStarted && newIsStarted && currentMenuState == PAUSE) {
      wantedMenuState = IN_GAME;
      changeInterface();
    }
    else if (isStarted && !newIsStarted && currentMenuState == IN_GAME) {
      wantedMenuState = PAUSE;
      changeInterface();
    }
    if (newIsStarted != isStarted){
      isStarted = newIsStarted;
      isStarted_CHANGED = true;
    }

    // Cards
    myNbCards = 0;
    for (JsonObjectConst cardData : root["cards"].as<JsonArrayConst>()) {

        if (myNbCards >= 30) {
            break;
        }

        Card& dst = myCards[myNbCards];
        int oldId = dst.id;

        newPointerCardFromJson(cardData, &dst);

        cards_CHANGED |= (oldId != dst.id);

        myNbCards++;
    }

    // Players
    nbPlayers = 0;
    for (JsonObjectConst playerData : root["players"].as<JsonArrayConst>()) {
      if (nbPlayers >= 4) {
        break;
      }

      Player& dst = playerList[nbPlayers];

      int oldId = dst.id;
      int oldNbCards = dst.nbCards;

      newPointerPlayerFromJson(playerData, &dst);

      if (nbPlayers == currentPlayerIndex) {
        playingPlayerId = dst.id;
      }

      if (dst.id == myId && dst.nbCards != myNbCards){
        nbCards_CHANGED = true;
        myNbCards = dst.nbCards;
      }

      players_CHANGED |= (oldId != dst.id || oldNbCards != dst.nbCards);

      nbPlayers++;
    }

    // UI updates
    if (lastCard_CHANGED){
      updatePile();
    }
    if (cards_CHANGED){
      updateCards();
    }
    if (nbCards_CHANGED){
      updateNbCards();
    }
    if (players_CHANGED || currentPlayerIndex_CHANGED){
      updatePlayers();
    }
    if (gameCode_CHANGED){
      updatePauseGameCode();
    }
    if (isStarted_CHANGED){
      updatePauseMenu();
    }
  }
}