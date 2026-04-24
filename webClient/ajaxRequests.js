/* IP adress of the serveur */
var serveurIp = "10.8.2.55:42069";

/* Get the connexion to the server */
function getConnexion() {
    $.ajax({
        type: 'GET',
        url: 'http://10.8.2.55:42069/index.html',
        success: function (reponse) { console.log(reponse) },
        error: function () { alert("getConnexion is not working ") }
    })
}



function getGameState() {
    $.ajax({
        type: 'GET',
        url: 'http://'+serveurIp+'/getGameState.c?userId=1&userPwd=enorme',
        success: function (reponse) { console.log(reponse) },
        error: function () { alert("getGameState is not working") }
    })
}

const ajaxRequests = {
    getConnexion,
    getGameState
};
