/*
---------- STRUCT BUILDERS ------------------------------------------------------------------------------------------
*/
Card newCardFromJson(JsonObject cardData){
  return {
    .id = cardData["cardId"].as<int>(),
    .typeId = cardData["cardTypeId"].as<int>(),
    .colorId = cardData["cardColorId"].as<int>(),
    .value = cardData["cardValue"].as<int>(),
    .colorHex = cardData["cardColorHex"].as<String>(),
    .typeDescription = cardData["cardTypeDesc"].as<String>()
  };
}
Player newPlayerFromJson(JsonObject cardData){
  return {
    .id = cardData["playerId"].as<int>(),
    .username = cardData["username"].as<String>(),
    .nbCards = cardData["cardCount"].as<int>()
  };
}

/*
---------- COLOR CONVERTION ------------------------------------------------------------------------------------------
*/
byte hexCharToByte(char hex){
  if ( '0' <= hex && hex <= '9'){
    return (hex - '0');
  }else if ('a' <= hex && hex <= 'z'){
    return (hex - 'a' + 10);
  }else if ('A' <= hex && hex <= 'Z'){
    return (hex - 'A' + 10);
  }else{
    return 255;
  }
}

uint16_t colorFromHex(String hex){
  byte r = 16*hexCharToByte(hex.charAt(0)) + hexCharToByte(hex.charAt(1));
  byte g = 16*hexCharToByte(hex.charAt(2)) + hexCharToByte(hex.charAt(3));
  byte b = 16*hexCharToByte(hex.charAt(4)) + hexCharToByte(hex.charAt(5));
  return tft.color565(r,g,b);
}