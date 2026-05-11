/* -------------------------------------------- PARAMETERS ------------------------------------------------ */
// Get the parameters of the user, send from the login page
let params = new URLSearchParams(window.location.search);

let userId = params.get("userId");
let userPwd = params.get("userPwd");
let username = params.get("username");

let currentUrl = window.location.href;

// Try the connexion of debuging
ajaxRequests.getConnexion();



/* -------------------------------------------- JOIN GAME ------------------------------------------------ */
function joinGame(code) {
    ajaxRequests.joinGame(userId, userPwd, code).then(r => {
        result = r;
        if(result.ok) {
            alert("Join game!!!");
            window.location.replace(currentUrl+"/../lobby.html?userId="+userId+"&userPwd="+userPwd+"&username="+username+"&code="+code);
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
            joinGame(result.code);
            alert("Code : "+result.code);
            //window.location.replace(currentUrl+"/../lobby.html?userId="+userId+"&userPwd="+userPwd+"&username="+username+"&code="+result.code);
        }else {
            alert(result.error);
        }
    });
}




/* Calling the functions for the right buttons */
$("#usernameDisplay").html(username);
$("#joinGameButton").click(()=>joinGame($("#gameCode").val()));
$("#createGameButton").click(createGame);

