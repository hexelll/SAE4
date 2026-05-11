/*
---------- DISPLAY UTILS ------------------------------------------------------------------------------------------
*/
// Display individual card
// MAINLY CHAT GPT CODE FOR DISPLAYING INDIVIDUAL CARDS BECAUSE ALIGNEMENT IS HELL
void displayCard(short x, short y, short sx, short margin, Card cardToDisplay){
    uint16_t color = colorFromHex(cardToDisplay.colorHex);
    String value;
    if (cardToDisplay.typeDescription == "normal"){
      value = String(cardToDisplay.value);
    }else if (cardToDisplay.typeDescription == "plus"){
      value = "+"+String(cardToDisplay.value);
    }else if (cardToDisplay.typeDescription == "reverse"){
      value = "R";
    }else if (cardToDisplay.typeDescription == "pluswild"){
      value = "+4";
    }else if (cardToDisplay.typeDescription == "skip"){
      value = "(/)";
    }else if (cardToDisplay.typeDescription == "wild"){
      value = "[?]";
    }else {
      value = cardToDisplay.typeDescription;
    }
    
    short w = sx;
    short h = sx * 3 / 2;

    short innerW = w - 2 * margin;
    short innerH = h - 2 * margin;

    // outer card
    tft.fillRoundRect(x, y, w, h, sx/6, ILI9341_WHITE);
    tft.drawRoundRect(x, y, w, h, sx/6, ILI9341_BLACK);

    // inner card
    tft.fillRoundRect(
        x + margin,
        y + margin,
        innerW,
        innerH,
        sx/8,
        color
    );

    // optional clamp
    /*
    if (value.length() > 3) {
        value = value.substring(0,3);
    }
    */

    // auto text size
    short textSize = getBestTextSize(
        value,
        10,                 // max allowed size
        innerW - 4,         // padding
        innerH - 4
    );

    tft.setTextSize(textSize);

    // measure final text
    int16_t x1, y1;
    uint16_t tw, th;

    tft.getTextBounds(value, 0, 0, &x1, &y1, &tw, &th);

    // centered position
    int16_t textX = x + margin + (innerW - tw) / 2;
    int16_t textY = y + margin + (innerH - th) / 2;

    tft.setCursor(textX, textY);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(value);
}
short getBestTextSize(String text, short maxSize, short maxWidth, short maxHeight){
    int16_t x1, y1;
    uint16_t w, h;

    for (short s = maxSize; s >= 1; s--){
        tft.setTextSize(s);
        tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

        if (w <= maxWidth && h <= maxHeight){
            return s;
        }
    }
    return 1;
}

// displays player profile (on the left)
void displayPlayerProfile(short y,short nbCards,String username,uint16_t backgroundColor){  
  // background
  tft.fillRect(0, y, 80, 50, backgroundColor); 
  
  // image
  tft.fillRect(8, y+7, 24, 24, tft.color565(0, 255, 0)); 

  // side cards
  if (nbCards != 1){
    tft.fillRoundRect(38, y+12, 15, 19, 2, tft.color565(0, 0, 255));
    tft.drawRoundRect(38, y+12, 15, 19, 2, tft.color565(255, 255, 255));
  }
  if(nbCards > 2){
    tft.fillRoundRect(60, y+12, 15, 19, 2, tft.color565(0, 255, 0));
    tft.drawRoundRect(60, y+12, 15, 19, 2, tft.color565(255, 255, 255));
  }

  // main card with nbCards
  if (nbCards < 10){
    tft.fillRoundRect(47, y+7, 20, 25, 3, tft.color565(255, 0, 0));
    tft.drawRoundRect(47, y+7, 20, 25, 3, tft.color565(255, 255, 255));

    tft.setCursor(52, y+12);
    tft.setTextSize(2);
    tft.print(nbCards);
  }else{
    tft.fillRoundRect(44, y+7, 26, 25, 3, tft.color565(255, 0, 0));
    tft.drawRoundRect(44, y+7, 26, 25, 3, tft.color565(255, 255, 255));

    if (nbCards < 20){
      tft.setCursor(45, y+12);
    }else{
      tft.setCursor(46, y+12);
    }
    tft.setTextSize(2);
    tft.print(nbCards);
  }

  // username 
  String short_username;
  if ( username.length() > 11 ){
   short_username = username.substring(0,9)+"..";
  }else{
    short_username = username;
  }
  tft.setCursor(10, y+37);
  tft.setTextSize(1);
  tft.print(short_username);
}


/*
---------- GENERAL FUNCTIONS ------------------------------------------------------------------------------------------
*/
// change to wanted menu
void changeInterface(){
  if (currentMenuState != wantedMenuState){
    switch (wantedMenuState){
      case IN_GAME: {
        initGameInterface();
        break;
      }
      case PAUSE: {
        initPauseInterface();
        break;
      }
      case TITLE_SCREEN: {
        initTitleScreen();
        break;
      }
      case NONE: {
        tft.fillScreen(ILI9341_BLACK);
        break;
      }
    }
    currentMenuState = wantedMenuState;
  }
}

void updateActiveEffects(){
  static long lastUpdateTime = millis();
  static bool blinkState = true;

  long currentTime = millis();
  if ( currentTime - lastUpdateTime > 1500){
    lastUpdateTime = currentTime;
    blinkState = ! blinkState;
    switch (currentMenuState){
      case IN_GAME: {
      
        break;
      }
      case PAUSE: {
        
        break;
      }
      case TITLE_SCREEN: {
        if (blinkState){
          tft.setTextColor(ILI9341_YELLOW);
        }else{
          tft.setTextColor(tft.color565(255, 80, 0));
        }
        tft.setTextSize(3);
        tft.setCursor(65, 195);
        tft.print("Commencer!");
        tft.setTextColor(ILI9341_WHITE);
        break;
      }
    }
  }
}


/*
---------- TITLE_SCREEN INTERFACE ------------------------------------------------------------------------------------------
*/
void initTitleScreen(){
  tft.fillScreen(ILI9341_BLACK);

  // UNO logo
  tft.setTextSize(10);
  tft.setCursor(70, 20);
  tft.setTextColor(ILI9341_RED);
  tft.print("U");
  tft.setTextColor(ILI9341_GREEN);
  tft.print("N");
  tft.setTextColor(ILI9341_BLUE);
  tft.print("O");

  // splash text
  updateSplashText();
}
void showStartText(){
  // start text
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 140);
  tft.print("Appuyer sur n'importe");
  tft.setCursor(40, 165);
  tft.print("quelle touche pour");
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(3);
  tft.setCursor(65, 195);
  tft.print("Commencer!");
  tft.setTextColor(ILI9341_WHITE);
}
void updateSplashText(){
  // list of splash texts
  static String spashText[3] = {"Garanti sans Herobrine !","Le seul & l'UNO-ique","100% requetes paires"};
  
  static short nbSplashTexts = sizeof(spashText)/sizeof(spashText[0]);

  tft.fillRect(135,97,185,10,ILI9341_BLACK);

  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.setCursor(135, 97);
  tft.print( spashText[ random(nbSplashTexts) ] );
  tft.setTextColor(ILI9341_WHITE);
}

/*
---------- PAUSE INTERFACE ------------------------------------------------------------------------------------------
*/
void initPauseInterface(){
  if (currentMenuState == IN_GAME){
    tft.fillRect(80, 0, 250, 240, ILI9341_BLACK);
  }else{
    tft.fillScreen(ILI9341_BLACK);

    // build players
    tft.fillRect(0, 0, 80, 20, tft.color565(80, 80, 80));
    tft.setTextSize(1);
    tft.setCursor(17, 7);
    tft.print("Joueurs");
    tft.fillRect(0, 220, 80, 20, tft.color565(128, 128, 128));
    // fill players with data
    updatePlayers();
  }

  // Pause text
  tft.setTextSize(3);
  tft.setCursor(156, 3);
  tft.print("Pause");
}

void updatePauseMenu(){

}

/*
---------- IN_GAME INTERFACE ------------------------------------------------------------------------------------------
*/
void initGameInterface(){
  if (currentMenuState == PAUSE){
    tft.fillRect(80, 0, 250, 240, ILI9341_BLACK);
  }else{
    tft.fillScreen(0x0000);

    // build players
    tft.fillRect(0, 0, 80, 20, tft.color565(80, 80, 80));
    tft.setTextSize(1);
    tft.setCursor(17, 7);
    tft.print("Joueurs");
    tft.fillRect(0, 220, 80, 20, tft.color565(128, 128, 128));
    // fill players with data
    updatePlayers();
  }

  // build cards in hand
  tft.setTextSize(1);
  tft.setCursor(275, 228);
  tft.print("Cartes");
  // fill cards in hand with data
  updateCards();

  // build pile
  tft.setTextSize(2);
  tft.setCursor(253, 5);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Pile");
  // fill pile with data
  updatePile();
}

void updatePlayers(){
  if (wantedMenuState == IN_GAME || wantedMenuState == PAUSE){
    static uint16_t backgroundColors[] = {tft.color565(128, 128, 128), tft.color565(160, 160, 160)};
    short y = 20;
    uint16_t backgroundColor;
    for (short i=0; i<4; i++){
      backgroundColor = backgroundColors[i%2];

      if (nbPlayers > i){
        Player playerToDisplay = playerList[i];
        displayPlayerProfile(y,playerToDisplay.nbCards,playerToDisplay.username,backgroundColor);
      }else{
        tft.fillRect(0, y, 80, 50, backgroundColor); 
      }
      y += 50;
    }

    if (nbPlayers > 4){
      tft.setTextSize(1);
      tft.setCursor(5, 227);
      tft.print("et ");
      tft.print(nbPlayers-4);
      tft.print(" autres");
    }
  }
}

void updateCards(){
  if (wantedMenuState == IN_GAME){
    // nb cartes
    tft.fillRect(270,185,50,40,ILI9341_BLACK);
    tft.setTextSize(4);
    if (myNbCards >= 10){
      tft.setCursor(270, 195);
    }else{
      tft.setCursor(281, 195);
    }
    tft.print(myNbCards);

    // right cards
    if ( !( positionInHand+3 >= myNbCards) ){
      displayCard(205,190,60,5,myCards[positionInHand+3]); 
    }else{
      tft.fillRoundRect(205, 190, 60, 60*3/2, 60/6, ILI9341_BLACK);
    }
    if ( !( positionInHand+2 >= myNbCards) ){
      displayCard(180,170,70,5,myCards[positionInHand+2]); 
    }else{
      tft.fillRoundRect(180, 170, 70, 70*3/2, 70/6, ILI9341_BLACK);
    }
    if ( !( positionInHand+1 >= myNbCards) ){
      displayCard(150,150,80,5,myCards[positionInHand+1]); 
    }else{
      tft.fillRoundRect(150, 150, 80, 80*3/2, 80/6, ILI9341_BLACK);
    }

    // left card
    if (positionInHand > 0){
      displayCard(85,150,80,5,myCards[positionInHand-1]);
    }else{
      tft.fillRoundRect(85, 150, 80, 80*3/2, 80/6, ILI9341_BLACK);
    }
    
    // Biggest card
    //displayCard(112,125,90,6,myCards[positionInHand]); 
    if (myNbCards > 0 &&
      positionInHand >= 0 &&
      positionInHand < myNbCards) {
      displayCard(112,125,90,6,myCards[positionInHand]);
    } else {
      tft.fillRoundRect(112, 125, 90, 90*3/2, 90/6, ILI9341_BLACK);
    }
  }
}

void updatePile(){
  if (wantedMenuState == IN_GAME){
    displayCard(235,25,80,6,lastPlayedCard);
  }
}
