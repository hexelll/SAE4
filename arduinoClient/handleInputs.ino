#define TIME_BETWEEN_JOYSTICK_INPUTS 160

void handleInputs(){
  static bool WAS_PRESSED_MENU = false;
  static bool WAS_PRESSED_OK = false;
  static bool WAS_PRESSED_NO = false;
  static bool WAS_PRESSED_UNO = false;
  static bool WAS_PRESSED_JOYSTICK = false;

  static long timeLastJoystickAction = millis();

  bool is_pressed_menu = !digitalRead(2);
  bool is_pressed_ok = !digitalRead(3);
  bool is_pressed_no = !digitalRead(4);
  bool is_pressed_uno = !digitalRead(5);
  bool is_pressed_joystick = !digitalRead(7);
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
        }
      }

      // ok button plays card
      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          int idCardToPlay = myCards[positionInHand].id;
          queueNewRequest("GET","/playCard.c?userId="+String(myId)+"&userPwd="+myPassword+"&cardId="+String(idCardToPlay),"", callback_errorHandler );
          if (positionInHand == myNbCards - 1 && positionInHand > 0){
            positionInHand--;
            updateCards();
          }
          timeGetGameState = millis();
          queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_getGameState );
        }
      }

      // no button draws a card
      if ( is_pressed_no ){
        WAS_PRESSED_NO = true;
      }else{
        if (WAS_PRESSED_NO){
          WAS_PRESSED_NO = false;
          queueNewRequest("GET","/drawCard.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_errorHandler );
          timeGetGameState = millis();
          queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_getGameState );
        }
      }

      if ( is_pressed_uno ){
        WAS_PRESSED_UNO = true;
      }else{
        if (WAS_PRESSED_UNO){
          WAS_PRESSED_UNO = false;
          if (playingPlayerId == myId){
            queueNewRequest("GET","/declareUno.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_errorHandler );
          }else{
            queueNewRequest("GET","/counterUno.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_errorHandler );
          }
        }
      }

      // use joystick to scroll cards
      if ( joystick_y < 75 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInHand < myNbCards - 1){
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
      break;
    }
    case MAIN: {
      // joystick button changes the splash text
      if ( is_pressed_joystick ){
        WAS_PRESSED_JOYSTICK = true;
      }else{
        if (WAS_PRESSED_JOYSTICK){
          WAS_PRESSED_JOYSTICK = false;
          updateSplashText();
        }
      }

      // no button draws a card
      if ( is_pressed_no ){
        WAS_PRESSED_NO = true;
      }else{
        if (WAS_PRESSED_NO){
          WAS_PRESSED_NO = false;
          wantedMenuState = TITLE_SCREEN;
        }
      }

      // ok button plays card
      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          if (positionInMenu == 0){
            wantedMenuState = JOIN_GAME;
          }else{
            wantedMenuState = CREATE_GAME;
          }
        }
      }

      if ( joystick_x > 950 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInMenu < 1){
          positionInMenu++;
          updateMainMenu();
        }
      }
      if ( joystick_x < 75&& currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInMenu > 0){
          positionInMenu--;
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
          }
        }
      }

      break;
    }
    case JOIN_GAME: {
      
      if ( joystick_y < 75 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInGameCode < 5){
          eraseInputGameCode();
          positionInGameCode++;
          updateInputGameCode();
        }
      }
      if ( joystick_y > 950 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (positionInGameCode > 0){
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
      if ( joystick_x > 950 && currentTime - timeLastJoystickAction > TIME_BETWEEN_JOYSTICK_INPUTS ){
        timeLastJoystickAction = currentTime;
        if (inputGameCode[positionInGameCode] > 0){
          inputGameCode[positionInGameCode]--;
          updateInputGameCode();
        }
      }
      
      // no button draws a card
      if ( is_pressed_no ){
        WAS_PRESSED_NO = true;
      }else{
        if (WAS_PRESSED_NO){
          WAS_PRESSED_NO = false;
          wantedMenuState = MAIN;
        }
      }

      // ok button plays card
      if ( is_pressed_ok ){
        WAS_PRESSED_OK = true;
      }else{
        if (WAS_PRESSED_OK){
          WAS_PRESSED_OK = false;
          String code = "";
          for (byte i=0 ; i<6 ; i++){
            code += String(inputGameCode[i]);
          }
          queueNewRequest("GET","/joinGame.c?userId="+String(myId)+"&userPwd="+myPassword+"&gameCode="+code,"", callback_joinGame );
        }
      }
      
      /*
      if (is_pressed_ok ){ // TEMP
        wantedMenuState = PAUSE;
        queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_getGameState );
      }
      */

      break;
    }
    case CREATE_GAME: {
      // no button draws a card
      if ( is_pressed_no ){
        WAS_PRESSED_NO = true;
      }else{
        if (WAS_PRESSED_NO){
          WAS_PRESSED_NO = false;
          wantedMenuState = MAIN;
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
          updateSplashText();
        }
      }

      // any button changes to IN_GAME interface
      if ( is_pressed_menu || is_pressed_ok || is_pressed_no || is_pressed_uno ){
        wantedMenuState = MAIN;
        //queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_getGameState );
      }

      break;
    }
  }
}