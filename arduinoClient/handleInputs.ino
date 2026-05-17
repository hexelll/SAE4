#define TIME_BETWEEN_JOYSTICK_INPUTS 160

void handleInputs(){
  static bool WAS_PRESSED_MENU = false;
  static bool WAS_PRESSED_OK = false;
  static bool WAS_PRESSED_NO = false;
  static bool WAS_PRESSED_UNO = false;
  static bool WAS_PRESSED_JOYSTICK = false;

  static long timeLastJoystickAction = millis();

  bool is_pressed_menu = !digitalRead(3);
  bool is_pressed_ok = !digitalRead(4);
  bool is_pressed_no = !digitalRead(5);
  bool is_pressed_uno = !digitalRead(2);
  bool is_pressed_joystick = !digitalRead(8);
  short joystick_x = analogRead(A0);
  short joystick_y = analogRead(A1);

  long currentTime = millis();

  switch (currentMenuState){
    case IN_GAME: {
      // menu button changes to PAUSE interface
      if ( is_pressed_menu ){
        WAS_PRESSED_MENU = true;
      }else{
        if (WAS_PRESSED_MENU){
          WAS_PRESSED_MENU = false;
          wantedMenuState = PAUSE;
          isInputingWild = false;
          tone(7,340,100);
        }
      }

      if (isInputingWild){

        // no button draws a card
        if ( is_pressed_no ){
          WAS_PRESSED_NO = true;
        }else{
          if (WAS_PRESSED_NO){
            WAS_PRESSED_NO = false;
            isInputingWild = false;
            updateCards();
            tone(7,140,100);
          }
        }

        if ( joystick_y < 75 ){
          queueNewRequest("GET","/playCard.c?userId="+String(myId)+"&userPwd="+myPassword+"&cardId="+String(myCards[positionInHand].id)+"&cardColorId=4", callback_errorHandler );
          timeGetGameState = millis();
          queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
          isInputingWild = false;
          tone(7,240,100);
          updateCards();
        }else if(joystick_y > 950){
          queueNewRequest("GET","/playCard.c?userId="+String(myId)+"&userPwd="+myPassword+"&cardId="+String(myCards[positionInHand].id)+"&cardColorId=2", callback_errorHandler );
          timeGetGameState = millis();
          queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
          isInputingWild = false;
          tone(7,240,100);
          updateCards();
        }else if(joystick_x > 950){
          queueNewRequest("GET","/playCard.c?userId="+String(myId)+"&userPwd="+myPassword+"&cardId="+String(myCards[positionInHand].id)+"&cardColorId=3", callback_errorHandler );
          timeGetGameState = millis();
          queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
          isInputingWild = false;
          tone(7,240,100);
          updateCards();
        }else if(joystick_x < 75){
          queueNewRequest("GET","/playCard.c?userId="+String(myId)+"&userPwd="+myPassword+"&cardId="+String(myCards[positionInHand].id)+"&cardColorId=1", callback_errorHandler );
          timeGetGameState = millis();
          queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
          isInputingWild = false;
          tone(7,240,100);
          updateCards();
        }

      }else{

        // use joystick to scroll cards
        if ( joystick_y < 75 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
          timeLastJoystickAction = currentTime;
          if (positionInHand < min(myNbCards - 1 , 30) ){
            positionInHand++;
            updateCards();
          }
        }
        if ( joystick_y > 950 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
          timeLastJoystickAction = currentTime;
          if (positionInHand > 0){
            positionInHand--;
            updateCards();
          }
        }

        // no button draws a card
        if ( is_pressed_no ){
          WAS_PRESSED_NO = true;
        }else{
          if (WAS_PRESSED_NO){
            WAS_PRESSED_NO = false;
            queueNewRequest("GET","/drawCard.c?userId="+String(myId)+"&userPwd="+myPassword, callback_errorHandler );
            timeGetGameState = millis();
            queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
            tone(7,140,100);
          }
        }
      }

      // ok button plays card
      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          Card cardToPlay = myCards[positionInHand];
          if (cardToPlay.typeDescription.equals("wild") || cardToPlay.typeDescription.equals("pluswild")){
            isInputingWild = true;
            updateCards();
          }else{
            queueNewRequest("GET","/playCard.c?userId="+String(myId)+"&userPwd="+myPassword+"&cardId="+String(cardToPlay.id), callback_errorHandler );
            if (positionInHand == min(myNbCards - 1 , 30) && positionInHand > 0){
              positionInHand--;
              updateCards();
            }
            timeGetGameState = millis();
            queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
            tone(7,240,100);
          }
        }
      }

      if ( is_pressed_uno ){
        WAS_PRESSED_UNO = true;
      }else{
        if (WAS_PRESSED_UNO){
          WAS_PRESSED_UNO = false;
          if (playingPlayerId == myId){
            queueNewRequest("GET","/declareUno.c?userId="+String(myId)+"&userPwd="+myPassword, callback_errorHandler );
            timeGetGameState = millis();
            queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
          }else{
            queueNewRequest("GET","/counterUno.c?userId="+String(myId)+"&userPwd="+myPassword, callback_errorHandler );
            timeGetGameState = millis();
            queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
          }
        }
      }


      break;
    }
    case MAIN: {
      // joystick button changes the splash text
      if ( is_pressed_joystick ){
        WAS_PRESSED_JOYSTICK = true;
      }else{
        if (WAS_PRESSED_JOYSTICK){
          WAS_PRESSED_JOYSTICK = false;
          tone(7,440,80);
          updateSplashText();
        }
      }

      if ( is_pressed_no ){
        WAS_PRESSED_NO = true;
      }else{
        if (WAS_PRESSED_NO){
          WAS_PRESSED_NO = false;
          wantedMenuState = TITLE_SCREEN;
          tone(7,140,100);
        }
      }

      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          if (positionInMenu == 0){
            wantedMenuState = JOIN_GAME;
          }else{
            wantedMenuState = CREATE_GAME;
            creatorId = myId;
            queueNewRequest("GET","/createGame.c?userId="+String(myId)+"&userPwd="+myPassword, callback_createGame );
          }
          tone(7,240,100);
        }
      }

      if ( joystick_x < 75 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInMenu > 0){
          positionInMenu--;
          updateMainMenu();
        }
      }
      if ( joystick_x > 900 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInMenu < 1){
          positionInMenu++;
          updateMainMenu();
        }
      }
      break;
    }
    case PAUSE: {
      // menu button changes to IN_GAME interface
      if ( is_pressed_menu ){
        WAS_PRESSED_MENU = true;
      }else{
        if (WAS_PRESSED_MENU){
          WAS_PRESSED_MENU = false;
          if (isStarted){
            wantedMenuState = IN_GAME;
            tone(7,340,100);
          }
        }
      }

      if ( joystick_x > 900 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInMenu < 1){
          positionInMenu++;
          updatePauseMenu();
        }
      }
      if ( joystick_x < 75 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInMenu > 0){
          positionInMenu--;
          updatePauseMenu();
        }
      }

      // ok button plays card
      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          if (positionInMenu == 0){
            if (isStarted){
              queueNewRequest("GET","/stopGame.c?userId="+String(myId)+"&userPwd="+myPassword, callback_errorHandler );
              timeGetGameState = millis();
              queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
            }else{
              queueNewRequest("GET","/startGame.c?userId="+String(myId)+"&userPwd="+myPassword, callback_errorHandler );
              timeGetGameState = millis();
              queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword, callback_getGameState );
              updatePauseMenu();
            }
            tone(7,240,100);
          }else{
            queueNewRequest("GET","/stopGame.c?userId="+String(myId)+"&userPwd="+myPassword, callback_errorHandler );
            queueNewRequest("GET","/deleteGame.c?userId="+String(myId)+"&userPwd="+myPassword, callback_deleteGame );
            tone(7,140,100);
          }
        }
      }

      break;
    }
    case JOIN_GAME: {
      
      if ( joystick_y < 75 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInGameCode < 5){
          tone(7,80,60);
          eraseInputGameCode();
          positionInGameCode++;
          updateInputGameCode();
        }
      }
      if ( joystick_y > 950 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInGameCode > 0){
          tone(7,80,60);
          eraseInputGameCode();
          positionInGameCode--;
          updateInputGameCode();
        }
      }

      if ( joystick_x < 75 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if ( inputGameCode[positionInGameCode] < 9){
          inputGameCode[positionInGameCode]++;
          updateInputGameCode();
        }
      }
      if ( joystick_x > 900 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (inputGameCode[positionInGameCode] > 0){
          inputGameCode[positionInGameCode]--;
          updateInputGameCode();
        }
      }
      
      if ( is_pressed_no ){
        WAS_PRESSED_NO = true;
      }else{
        if (WAS_PRESSED_NO){
          WAS_PRESSED_NO = false;
          wantedMenuState = MAIN;
          tone(7,140,100);
        }
      }

      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          eraseJoinError();
          tone(7,240,100);
          fuseGameId();
          queueNewRequest("GET","/joinGame.c?userId="+String(myId)+"&userPwd="+myPassword+"&gameCode="+gameCode, callback_joinGame );
        }
      }

      break;
    }
    case CREATE_GAME: {

      // no button draws a card
      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          wantedMenuState = PAUSE;
          tone(7,240,100);
        }
      }

      break;
    }
    case TITLE_SCREEN: {
      // joystick button changes the splash text
      if ( is_pressed_joystick ){
        WAS_PRESSED_JOYSTICK = true;
      }else{
        if (WAS_PRESSED_JOYSTICK){
          WAS_PRESSED_JOYSTICK = false;
          tone(7,440,80);
          updateSplashText();
        }
      }

      // no button draws a card
      if ( is_pressed_no ){
        WAS_PRESSED_NO = true;
      }else{
        if (WAS_PRESSED_NO){
          WAS_PRESSED_NO = false;
          wantedMenuState = MAIN;
          tone(7,240,100);
        }
      }

      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          wantedMenuState = MAIN;
          tone(7,240,100);
        }
      }

      if ( is_pressed_uno ){
        WAS_PRESSED_UNO = true;
      }else{
        if (WAS_PRESSED_UNO){
          WAS_PRESSED_UNO = false;
          wantedMenuState = MAIN;
          tone(7,240,100);
        }
      }

      if ( is_pressed_menu ){
        WAS_PRESSED_MENU = true;
      }else{
        if (WAS_PRESSED_MENU){
          WAS_PRESSED_MENU = false;
          wantedMenuState = MAIN;
          tone(7,240,100);
        }
      }
      
      break;
    }
  }
}