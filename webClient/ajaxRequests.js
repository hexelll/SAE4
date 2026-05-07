/* IP adress of the serveur */
var serveurIpAlex = "10.8.2.55:42069";
var serveurIpLuc = "10.8.19.132:42069";
var serveurIpMoi = "localhost:42069";

var serveurIp = serveurIpMoi;


/* Get the connexion to the server */
function getConnexion() {
    $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/index.html',
        success: function (connexion) { console.log(connexion) },
        error: function () { alert("getConnexion is not working ") }
    })
}



async function login() {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/checkUserCredentials.c?',
        data: 'username='+$("#username").val()+'&userPwd='+$("#userPwd").val(),
        dataType: 'json',
        success: function (r) { 
            result = r; 
            userPwd= $("#userPwd").val();
            userId = result.id;
            console.log(userId);
        },
        error: function () { alert("checkUserCredentials is not working") }
    })
    
    return result;
}



async function getGameState(userId,userPwd) {
    let gameState;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/getGameState.c?userId='+userId+'&userPwd='+userPwd,
        dataType: 'json',
        success: function (g) { gameState = g; },
        error: function () { alert("getGameState is not working") }
    })
    return gameState;
}


async function playCard(userId,userPwd,cardId) {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/playCard.c?userId='+userId+'&userPwd='+userPwd+'&cardId='+cardId,
        dataType: 'json',
        success: function (r) { result = r; },
        error: function () { alert("playCard is not working") }
    })
    return result;
}


async function drawCard(userId,userPwd) {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/drawCard.c?userId='+userId+'&userPwd='+userPwd+'&cardId=',
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
    drawCard,
    login
};

