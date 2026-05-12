/* IP adress of the serveur */
var serveurIpAlex = "10.8.16.218:42069";
var serveurIpLuc = "10.8.30.27:42069";
var serveurIpMoi = "localhost:42069";

var serveurIp = serveurIpMoi;


/* -------------------------------------------- TRY CONNEXION ------------------------------------------------ */
function getConnexion() {
    $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/index.html',
        success: function (connexion) { console.log(connexion) },
        error: function () { alert("getConnexion is not working ") }
    })
}




/* -------------------------------------------- LOGIN  ------------------------------------------------ */
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


async function createAccount() {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/createAccount.c?',
        data: 'username='+$("#username").val()+'&userPwd='+$("#userPwd").val(),
        dataType: 'json',
        success: function (r) { 
            result = r; 
            /*
            userPwd= $("#userPwd").val();
            userId = result.id;
            console.log(userId);
            */
        },
        error: function () { alert("createAccount is not working") }
    })
    
    return result;
}




/* -------------------------------------------- MENU  ------------------------------------------------ */
async function joinGame(userId, userPwd, code) {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/joinGame.c?',
        data: 'userId='+userId+'&userPwd='+userPwd+'&gameCode='+code,
        dataType: 'json',
        success: function (r) { 
            result = r; 
            console.log(userId);
        },
        error: function () { alert("joinGame is not working") }
    })
    
    return result;
}

async function createGame(userId, userPwd) {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/createGame.c?',
        data: 'userId='+userId+'&userPwd='+userPwd,
        dataType: 'json',
        success: function (r) { result = r; },
        error: function () { alert("createGame is not working") }
    })
    return result;
}




/* -------------------------------------------- LOBBY ------------------------------------------------ */
async function startGame(userId, userPwd) {
    let result;
    await $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/startGame.c?',
        data: 'userId='+userId+'&userPwd='+userPwd,
        dataType: 'json',
        success: function (r) { 
            result = r; 
            //console.log(userId);
        },
        error: function () { alert("startGame is not working") }
    })
    
    return result;
}






/* -------------------------------------------- GAME  ------------------------------------------------ */
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




/* -------------------------------------------- CONST ------------------------------------------------ */
const ajaxRequests = {
    getConnexion,
    getGameState,
    playCard,
    drawCard,
    login,
    createAccount,
    joinGame,
    createGame,
    startGame
};

