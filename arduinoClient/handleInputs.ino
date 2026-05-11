void handleInputs(){
  static bool WAS_PRESSED_MENU = false;
  static bool WAS_PRESSED_OK = false;
  static bool WAS_PRESSED_NO = false;
  static bool WAS_PRESSED_UNO = false;
  static bool WAS_PRESSED_JOYSTICK = false;

  bool is_pressed_menu = !digitalRead(2);
  bool is_pressed_ok = !digitalRead(3);
  bool is_pressed_no = !digitalRead(4);
  bool is_pressed_uno = !digitalRead(5);
  bool is_pressed_joystick = !digitalRead(7);
  short joystick_x = analogRead(A0);
  short joystick_y = analogRead(A1);

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

      // use joystick to scroll cards
      if ( joystick_y < 75 ){
        if (positionInHand < myNbCards - 1){
          positionInHand++;
          updateCards();
        }
      }
      if ( joystick_y > 950 ){
        if (positionInHand > 0){
          positionInHand--;
          updateCards();
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
          wantedMenuState = IN_GAME;
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
        wantedMenuState = IN_GAME;
        queueNewRequest("GET","/getGameState.c?userId="+String(myId)+"&userPwd="+myPassword,"", callback_getGameState );
      }

      break;
    }
  }
}