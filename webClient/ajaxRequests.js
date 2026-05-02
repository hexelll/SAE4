/* IP adress of the serveur */
var serveurIpAlex = "10.8.2.55:42069";
var serveurIpLuc = "10.8.19.132:42069";


/* Get the connexion to the server */
function getConnexion() {
    $.ajax({
        type: 'GET',
        url: 'http://'+serveurIpAlex+'/index.html',
        success: function (connexion) { console.log(connexion) },
        error: function () { alert("getConnexion is not working ") }
    })
}



async function getGameState() {
    let gameState;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIpAlex+'/getGameState.c?userId=1&userPwd=enorme',
        dataType: 'json',
        success: function (g) { gameState = g; },
        error: function () { alert("getGameState is not working") }
    })
    return gameState;
}


async function playCard(cardId) {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIpAlex+'/playCard.c?userId=1&userPwd=enorme&cardId='+cardId,
        dataType: 'json',
        success: function (r) { result = r; },
        error: function () { alert("playCard is not working") }
    })
    return result;
}

async function drawCard() {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIpAlex+'/drawCard.c?userId=1&userPwd=enorme&cardId=',
        dataType: 'json',
        success: function (r) { result = r; },
        error: function () { alert("drawCard is not working") }
    })
    return result;
}

const ajaxRequests = {
    getConnexion,
    getGameState,
    playCard,
    drawCard
};

