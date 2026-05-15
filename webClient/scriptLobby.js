/* -------------------------------------------- PARAMETERS ------------------------------------------------ */
// Get the parameters of the user, send from the login page
let params = new URLSearchParams(window.location.search);

let userId = params.get("userId");
let userPwd = params.get("userPwd");
let username = params.get("username");
let code = params.get("code");
let winner = params.get("winner");



// Get the current url of the page, get the index of the "/" to not get parameters of the url and gthe actual page
let currentUrl = window.location.href.toString();
let lastIndexOfSlash = currentUrl.lastIndexOf('/');
let niceUrl = currentUrl.substring(0, lastIndexOfSlash);
//console.log("Current :" +currentUrl);
//console.log("Nice url :" +niceUrl);




/* -------------------------------------------- DISPLAY ALL THE PLAYERS OF THE GAME ------------------------------------------------ */
/* To display the result of getGameState every 3 seconds */
function periodicGetGameState(){
    displayPlayers();
    setTimeout(periodicGetGameState, 3000); 
}



function displayPlayers() {
    console.log(userId,userPwd);
    ajaxRequests.getGameState(userId,userPwd).then(g => {
        gameState = g;
        console.log(gameState);

        $("#gameNumber").html(gameState.gameCode);


        // If the gameState return ok = false, the game has been deleted by the owner, so we return to the menu page
        if(gameState.ok == false) {
            alert("The game has been deleted by the owner");
            let newUrl = niceUrl+"/menu.html?userId="+userId+"&userPwd="+userPwd+"&username="+username;
            //console.log(newUrl);
            window.location.replace(newUrl);
        }

        else {
            // Verify if the game is started, to go to the page of the game
            if(gameState.isStarted) {
                let newUrl = niceUrl+"/play.html?userId="+userId+"&userPwd="+userPwd+"&username="+username+"&code="+code;
                //console.log(newUrl);
                window.location.replace(newUrl);
            }
            else {
                // Get the ower ID
                gameOwnerId = gameState.creatorId;
                // To create html in the script file
                let playerHtml = "<h2 class='playerTitle'>Players</h2>";

                
                playersId = [];
                playersName = [];
                for(i = 0; i < gameState.players.length; i++) {
                    // To display the owner of th game in its own div
                    if(gameOwnerId == gameState.players[i].playerId) {
                        gameOwnerName = gameState.players[i].username;
                        if (gameOwnerId == userId) {
                            $("#idGameOwner").html("<b>"+(i+1)+"</b>");
                            $("#nameGameOwner").html("<b>"+gameOwnerName+"</b>");
                        }else {
                            $("#idGameOwner").html((i+1)+"");
                            $("#nameGameOwner").html(gameOwnerName);
                        }
                        //gameOwnerIndex = i;
                    }
                    // To display the other players in their div
                    else {
                        playersId.push(gameState.players[i].playerId);
                        playersName.push(gameState.players[i].username);
                        playerHtml += "<div class='lobbyPlayer'><div class='lobbyPlayerIn'>"
                        if (userId+"" == gameState.players[i].playerId) {
                            playerHtml += "<p class='playerId'><b>"+(i+1)+"</b></p>";
                            playerHtml += "<p class='playerName'><b>"+gameState.players[i].username+"</b></p>";
                        }else {
                            playerHtml += "<p class='playerId'>"+(i+1)+"</p>";
                            playerHtml += "<p class='playerName'>"+gameState.players[i].username+"</p>";
                        }
                        playerHtml += "</div></div>"
                    }
                }

                // Display all the players in the div
                $("#playersData").html(playerHtml); 

                // If the user connected is the owner of the game, he can start the game
                if (parseInt(userId) === gameOwnerId) {
                    //console.log("You are the owner : "+userId+" == "+gameOwnerId);
                    $("#startGameButton").removeAttr("hidden");
                    $("#deleteGameButton").removeAttr("hidden");
                }
                else {
                    console.log("Not the owner : "+userId+" != "+gameOwnerId);
                }
            }
        }

    });
};



/* -------------------------------------------- START GAME ------------------------------------------------ */
function startGame() {
    ajaxRequests.startGame(userId, userPwd).then(r => {
        result = r;
        if(result.ok) {
            //alert("Start game");
            let newUrl = niceUrl+"/play.html?userId="+userId+"&userPwd="+userPwd+"&username="+username+"&code="+code;
            //console.log(newUrl);
            window.location.replace(newUrl);
        }else {
            alert(result.error);
        }
    });
    
}


function deleteGame() {
    ajaxRequests.deleteGame(userId, userPwd, code).then(r => {
        result = r;
        if(result.ok) {
            alert("Delete game");
            let newUrl = niceUrl+"/menu.html?userId="+userId+"&userPwd="+userPwd+"&username="+username;
            //console.log(newUrl);
            window.location.replace(newUrl);
        }else {
            alert(result.error);
        }
    });
}



// Caliing the function that get the gameState every 3 seconds
periodicGetGameState();


/*Calling the functions for the right buttons 
$("#nameGameOwner").html(username);
$("#idGameOwner").html(userId);*/
$("#gameNumber").html(code);
$("#winner").html("Winner : "+winner);
$("#startGameButton").click(startGame);
$("#deleteGameButton").click(deleteGame);





