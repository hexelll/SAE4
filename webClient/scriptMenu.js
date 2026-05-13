/* -------------------------------------------- PARAMETERS ------------------------------------------------ */
// Get the parameters of the user, send from the login page
let params = new URLSearchParams(window.location.search);

let userId = params.get("userId");
let userPwd = params.get("userPwd");
let username = params.get("username");


// Get the current url of the page, get the index of the "/" to not get parameters of the url and gthe actual page
let currentUrl = window.location.href.toString();
let lastIndexOfSlash = currentUrl.lastIndexOf('/');
let niceUrl = currentUrl.substring(0, lastIndexOfSlash);
//console.log("Current :" +currentUrl);
//console.log("Nice url :" +niceUrl);


// Try the connexion of debuging
ajaxRequests.getConnexion();



/* -------------------------------------------- JOIN GAME ------------------------------------------------ */
function joinGame(code) {
    ajaxRequests.joinGame(userId, userPwd, code).then(r => {
        result = r;
        if(result.ok) {
            //alert("Join game!!!");
            let newUrl = niceUrl+"/lobby.html?userId="+userId+"&userPwd="+userPwd+"&username="+username+"&code="+code;
            //console.log(newUrl);
            window.location.replace(newUrl);
        }else {
            alert(result.error);
        }
    });
    
}



/* -------------------------------------------- CREATE GAME ------------------------------------------------ */
function createGame() {
    ajaxRequests.createGame(userId, userPwd).then(r => {
        result = r;
        if(result.ok) {
            //joinGame(result.code);
            //alert("Code : "+result.code);
            let newUrl = niceUrl+"/lobby.html?userId="+userId+"&userPwd="+userPwd+"&username="+username+"&code="+result.code;
            //console.log(newUrl);
            window.location.replace(newUrl);
        }else {
            alert(result.error);
        }
    });
}




/* Calling the functions for the right buttons */
$("#usernameDisplay").html(username);
$("#joinGameButton").click(()=>joinGame($("#gameCode").val()));
$("#createGameButton").click(createGame);

