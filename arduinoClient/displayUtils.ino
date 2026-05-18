/*
---------- DISPLAY UTILS ------------------------------------------------------------------------------------------
*/
// Display individual card
// MAINLY CHAT GPT CODE FOR DISPLAYING INDIVIDUAL CARDS BECAUSE ALIGNEMENT IS HELL
void displayCard(short x, short y, short sx, short margin, Card cardToDisplay){
    uint16_t color = cardToDisplay.colorHex;
    
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
void displayPlayerProfile(short y,short nbCards,String username,uint16_t backgroundColor, bool isLocalPlayer, bool isPlaying, bool isOwner){  
  // background
  tft.fillRect(0, y, 80, 50, backgroundColor); 
  
  // highlight if currently playing
  if (isPlaying){
    tft.fillRoundRect(5, y+4, 30, 30,2, ILI9341_RED); 
  }

  // "image"
  static uint16_t colors[8] = {tft.color565(101, 191, 136),tft.color565(0,255,0),tft.color565(0,0,255),tft.color565(255,255,0),tft.color565(255,0,255),tft.color565(0,255,255),tft.color565(255,128,0),tft.color565(137, 81, 41)};
  
  byte colorIndex = min( max( 0, floor( (hexCharToByte( username.charAt(0) )-10)/3)),7 );
  uint16_t IconColor = colors[colorIndex];
  tft.fillRect(8, y+7, 24, 17, 0x0000 ); 
  tft.fillRect(8, y+24, 24, 7, IconColor ); 
  tft.fillCircle(20, y+17, 7, IconColor ); 

  if (wantedMenuState == IN_GAME){
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
  }else if (wantedMenuState == PAUSE && isOwner){
    tft.fillTriangle(46,y+12,46,y+26,56,y+26,tft.color565(255, 255, 0));
    tft.fillTriangle(56,y+26,66,y+12,66,y+26,tft.color565(255, 255, 0));
    tft.fillTriangle(51,y+26,61,y+26,56,y+12,tft.color565(255, 255, 0));
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
  if (isLocalPlayer){
    tft.setTextColor(ILI9341_YELLOW);
  }else{
    tft.setTextColor(ILI9341_WHITE);
  }
  tft.print(short_username);
  tft.setTextColor(ILI9341_WHITE);
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
      case CREATE_GAME: {
        initCreateGame();
        break;
      }
      case MAIN: {
        initMainInterface();
        break;
      }
      case JOIN_GAME: {
        initJoinGameInterface();
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
  if ( currentTime - lastUpdateTime > 1000){
    lastUpdateTime = currentTime;
    blinkState = ! blinkState;
    switch (currentMenuState){
      case IN_GAME: {
        tft.fillRect(35, 220, 45, 20, tft.color565(128, 128, 128));
        tft.setTextSize(1);
        tft.setCursor(35, 227);
        tft.print(min(9999,ping));
        tft.print("ms");
        break;
      }
      case PAUSE: {
        tft.fillRect(35, 220, 45, 20, tft.color565(128, 128, 128));
        tft.setTextSize(1);
        tft.setCursor(35, 227);
        tft.print(min(9999,ping));
        tft.print("ms");
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
---------- CREATE_GAME INTERFACE ------------------------------------------------------------------------------------------
*/
void initCreateGame(){
  tft.fillScreen(ILI9341_BLACK);

  // top text
  tft.setTextSize(3);
  tft.setCursor(50, 10);
  tft.print("Partie cree !");
  tft.setTextSize(1);
  tft.setCursor(75, 43);
  tft.print("Rejoignez la avec le code :");

  // arrow 
  tft.fillRect(140,200,140,30,ILI9341_RED);
  tft.fillTriangle(280, 200, 280, 229, 310, 214, ILI9341_RED);
  tft.setTextSize(2);
  tft.setCursor(145, 207);
  tft.print("C'est parti!");
}
void updateGameCodeDisplay(){
  static uint16_t colors[4] = {ILI9341_RED,ILI9341_GREEN,ILI9341_BLUE,ILI9341_YELLOW};

  if (wantedMenuState == CREATE_GAME){
    // actual code
    tft.setTextSize(8);
    tft.setCursor(20, 105);
    for (byte i=0 ; i<6 ; i++){
      tft.setTextColor( colors[i%4] );
      tft.print( gameCode.charAt(i) );
    }
    tft.setTextColor(ILI9341_WHITE);
  }
}

/*
---------- MAIN INTERFACE ------------------------------------------------------------------------------------------
*/
void initMainInterface(){
  if (currentMenuState == TITLE_SCREEN){
    tft.fillRect(0, 110, 320, 130, ILI9341_BLACK);
  }else{
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

  updateMainMenu();

}
void updateMainMenu(){
  uint16_t color1 = tft.color565(128, 128, 128);
  uint16_t color2 = tft.color565(255, 0, 0);
  if (positionInMenu == 0){
    uint16_t temp = color2;
    color2 = color1;
    color1 = temp;
  }
  tft.fillRoundRect(29, 127, 256, 31,10,color1 );
  tft.setTextSize(2);
  tft.setCursor(37, 135);
  tft.print("Rejoindre une partie");

  tft.fillRoundRect(52, 179, 206, 31,10,color2 );
  tft.setTextSize(2);
  tft.setCursor(60, 187);
  tft.print("Creer une partie");
}


/*
---------- JOIN_GAME INTERFACE ------------------------------------------------------------------------------------------
*/
void initJoinGameInterface(){
  tft.fillScreen(ILI9341_BLACK);

  // top text
  tft.setTextSize(2);
  tft.setCursor(57, 10);
  tft.print("Joindre la partie");
  tft.setTextSize(1);
  tft.setCursor(43, 35);
  tft.print("Entrez le code de la partie a rejoindre");
 
  // char séparé de 36px
  // char de 30px de largeur
  // char de 42px de hauteur

  updateInputGameCode();
}
void updateInputGameCode(){
  static uint16_t colors[4] = {ILI9341_RED,ILI9341_GREEN,ILI9341_BLUE,ILI9341_YELLOW};

  // actual code
  tft.setTextSize(6);
  tft.setCursor(50, 121);
  for (byte i=0 ; i<6 ; i++){
    if (i == positionInGameCode){
      tft.fillRect( 50+i*36, 121, 30, 44, ILI9341_BLACK );
    }
    tft.setTextColor( colors[i%4] );
    tft.print( String(inputGameCode[i]) );
  }
  tft.setTextColor(ILI9341_WHITE);

  // numbers above
  tft.setTextColor(tft.color565(128, 128, 128));

  tft.fillRect( 56+positionInGameCode*36, 59, 15, 21, ILI9341_BLACK );
  if ( inputGameCode[positionInGameCode] >= 2 ){
    tft.setTextSize(3);
    tft.setCursor(56+positionInGameCode*36, 59);
    tft.print( inputGameCode[positionInGameCode]-2 );
  }

  tft.fillRect( 54+positionInGameCode*36, 85, 20, 28, ILI9341_BLACK );
  if ( inputGameCode[positionInGameCode] >= 1 ){
    tft.setTextSize(4);
    tft.setCursor(54+positionInGameCode*36, 85);
    tft.print( inputGameCode[positionInGameCode]-1 );
  }

  tft.fillRect( 54+positionInGameCode*36, 169, 20, 28, ILI9341_BLACK );
  if ( inputGameCode[positionInGameCode] <= 8 ){
    tft.setTextSize(4);
    tft.setCursor(54+positionInGameCode*36, 169);
    tft.print( inputGameCode[positionInGameCode]+1 );
  }

  tft.fillRect( 56+positionInGameCode*36, 203, 15, 21, ILI9341_BLACK );
  if ( inputGameCode[positionInGameCode] <= 7 ){
    tft.setTextSize(3);
    tft.setCursor(56+positionInGameCode*36, 203);
    tft.print( inputGameCode[positionInGameCode]+2 );
  }

}
void eraseInputGameCode(){
  tft.fillRect( 56+positionInGameCode*36, 59, 15, 21, ILI9341_BLACK );
  tft.fillRect( 54+positionInGameCode*36, 85, 20, 28, ILI9341_BLACK );
  tft.fillRect( 54+positionInGameCode*36, 169, 20, 28, ILI9341_BLACK );
  tft.fillRect( 56+positionInGameCode*36, 203, 15, 21, ILI9341_BLACK );
}
void displayJoinError(){
  if (currentMenuState == JOIN_GAME){
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.setCursor(140, 227);
    tft.print("Aucune partie avec ce code !");
    tft.setTextColor(ILI9341_WHITE);
  }
}
void eraseJoinError(){
  if (currentMenuState == JOIN_GAME){
    tft.fillRect(140, 227, 180, 13,ILI9341_BLACK );
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

  tft.setTextSize(2);
  tft.setCursor(30, 140);
  tft.print("Appuyer sur n'importe");
  tft.setCursor(40, 165);
  tft.print("quelle touche pour");
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
void displayWifiError(){
  if (currentMenuState == TITLE_SCREEN){
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.setCursor(220, 220);
    tft.print("Erreur de wifi");
    tft.setTextColor(ILI9341_WHITE);
  }
}
void eraseWifiError(){
  if (currentMenuState == TITLE_SCREEN){
    tft.fillRect(220, 220, 100, 20, 0x0000);
  }
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
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(17, 7);
    tft.print("Joueurs");
    tft.fillRect(0, 220, 80, 20, tft.color565(128, 128, 128));
  }
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(5, 227);
  tft.print("ping: ");

  // fill players with data
  updatePlayers();

  // Pause text
  tft.setTextSize(4);
  tft.setCursor(142, 5);
  tft.print("Pause");

  // display game code
  updatePauseGameCode();

  updatePauseMenu();
}

void updatePauseGameCode(){
  if (wantedMenuState == PAUSE && !gameCode.equals("") ){
    tft.fillRect(113, 55, 200, 40, ILI9341_BLACK);
    static uint16_t colors[4] = {ILI9341_RED,ILI9341_GREEN,ILI9341_BLUE,ILI9341_YELLOW};
    tft.setTextSize(5);
    tft.setCursor(113, 55);
    for (byte i=0 ; i<6 ; i++){
      tft.setTextColor( colors[i%4] );
      tft.print( gameCode.charAt(i) );
    }
    tft.setTextColor(ILI9341_WHITE);
  }
}

void updatePauseMenu(){
  if (wantedMenuState == PAUSE){
    if (creatorId == myId){
      uint16_t color1 = tft.color565(128, 128, 128);
      uint16_t color2 = tft.color565(255, 0, 0);
      if (positionInMenu == 0){
        uint16_t temp = color2;
        color2 = color1;
        color1 = temp;
      }
      tft.fillRoundRect(100, 127, 206, 31,10,color1 );
      tft.setTextSize(2);
      tft.setCursor(110, 135);
      if (!isStarted){
        tft.print("Demarer");
      }else{
        tft.print("Arreter");
      }

      tft.fillRoundRect(100, 179, 206, 31,10,color2 );
      tft.setTextSize(2);
      tft.setCursor(110, 187);
      tft.print("Supprimer");
    }else{
      if (isStarted==false){
        tft.fillRoundRect(100, 127, 206, 31,10,ILI9341_RED );
        tft.setTextSize(2);
        tft.setCursor(110, 135);
        tft.print("Quiter");
      }else{
        tft.fillRoundRect(100, 127, 206, 31,10,ILI9341_BLACK );
      }
      tft.fillRoundRect(100, 179, 206, 31,10,ILI9341_BLACK );
    }
  }
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
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(17, 7);
    tft.print("Joueurs");
    tft.fillRect(0, 220, 80, 20, tft.color565(128, 128, 128));
    
  }
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(5, 227);
  tft.print("ping: ");

  updatePlayers();

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

        Player playerToDisplay;
        if (isReversed){
          playerToDisplay = playerList[ ( (nbPlayers-i)+currentPlayerIndex ) % nbPlayers ];
        }else{
          playerToDisplay = playerList[ (i+currentPlayerIndex) % nbPlayers ];
        }

        bool playerIsPlaying = playingPlayerId == playerToDisplay.id;
        bool playerIsMe = myId == playerToDisplay.id;
        bool playerIsOwner = playerToDisplay.id == creatorId;
        displayPlayerProfile(y,playerToDisplay.nbCards,playerToDisplay.username,backgroundColor,playerIsMe,playerIsPlaying,playerIsOwner);

      }else{
        tft.fillRect(0, y, 80, 50, backgroundColor); 
      }
      y += 50;
    }

    /* unused because only 4 players in game
    if (nbPlayers > 4){
      tft.setTextSize(1);
      tft.setCursor(5, 227);
      tft.print("et ");
      tft.print(nbPlayers-4);
      tft.print(" autres");
    }
    */
  }
}

void updateNbCards(){
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
  }
}

void updateCards(){
  if (wantedMenuState == IN_GAME){
    
    updateNbCards();

    // right cards
    if ( !( positionInHand+3 >= min(myNbCards,31) ) ){
      displayCard(205,190,60,5,myCards[positionInHand+3]); 
    }else{
      tft.fillRoundRect(205, 190, 60, 60*3/2, 60/6, ILI9341_BLACK);
    }
    if ( !( positionInHand+2 >= min(myNbCards,31)) ){
      displayCard(180,170,70,5,myCards[positionInHand+2]); 
    }else{
      tft.fillRoundRect(180, 170, 70, 70*3/2, 70/6, ILI9341_BLACK);
    }
    if ( !( positionInHand+1 >= min(myNbCards,31)) ){
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
    if (isInputingWild){
      // outer card
      tft.fillRoundRect(112, 125, 90, 90*3/2, 90/6, ILI9341_WHITE);
      tft.drawRoundRect(112, 125, 90, 90*3/2, 90/6, ILI9341_BLACK);

      // empty inner card
      tft.fillRoundRect(
          112 + 6,
          125 + 6,
          90 - 2*6,
          90*3/2 -2*6,
          90/8,
          ILI9341_BLACK
      );

      // temp
      short decal_y = 18;
      short h = 14;
      short w = 28;

      short middle_x = 112+6+(90-2*6)/2;
      short middle_y = 125+6+(90*3/2-2*6)*2/4;

      // triangles
      tft.fillTriangle( middle_x, middle_y -h -decal_y , middle_x - w/2 , middle_y-decal_y , middle_x + w/2, middle_y-decal_y,ILI9341_RED );
      tft.fillTriangle( middle_x, middle_y +h +decal_y , middle_x - w/2 , middle_y+decal_y , middle_x + w/2, middle_y+decal_y,ILI9341_GREEN );

      tft.fillTriangle( middle_x-decal_y, middle_y+w/2 , middle_x-decal_y , middle_y-w/2 , middle_x-decal_y-h, middle_y,ILI9341_BLUE );
      tft.fillTriangle( middle_x+decal_y, middle_y+w/2 , middle_x+decal_y , middle_y-w/2 , middle_x+decal_y+h, middle_y, ILI9341_YELLOW );

      tft.drawRect(middle_x-10, middle_y-10, 20,20, ILI9341_WHITE);
      tft.fillRect(middle_x-6, middle_y-6, 12,12, ILI9341_WHITE);

    }else if (myNbCards > 0 &&
      positionInHand >= 0 &&
      positionInHand < min(myNbCards,31)) {
      displayCard(112,125,90,6,myCards[positionInHand]);
    }else{
      tft.fillRoundRect(112, 125, 90, 90*3/2, 90/6, ILI9341_BLACK);
    }
  }
}

void updatePile(){
  if (wantedMenuState == IN_GAME){
    displayCard(235,25,80,6,lastPlayedCard);
  }
}
