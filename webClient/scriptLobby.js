/* -------------------------------------------- PARAMETERS ------------------------------------------------ */
// Get the parameters of the user, send from the login page
let params = new URLSearchParams(window.location.search);

let userId = params.get("userId");
let userPwd = params.get("userPwd");
let username = params.get("username");
let code = params.get("code");


// Get the current url of the page, get the index of the "?" to not get parameters of the url
let url = window.location.href;
let currentUrl = url.substring(0, url.indexOf('?'));



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

        // Verify if the game is started, to go to the page of the game
        if(gameState.isStarted) {
            window.location.replace(currentUrl+"/../play.html?userId="+userId+"&userPwd="+userPwd+"&username="+username);
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
                    $("#idGameOwner").html("Player "+gameOwnerId + " :");
                    $("#nameGameOwner").html(gameOwnerName);
                    //gameOwnerIndex = i;
                }
                // To display the other players in their div
                else {
                    playersId.push(gameState.players[i].playerId);
                    playersName.push(gameState.players[i].username);
                    playerHtml += "<p class='playerId'> Player "+ i+" : </p>";
                    playerHtml += "<p class='playerName'>"+gameState.players[i].username+"</p>";
                    playerHtml += "<br>";
                }
            }

            // Display all the players in the div
            $("#playersData").html(playerHtml); 




            // If the user connected is the owner of the game, he can start the game
            if (userId === gameOwnerId) {
                console.log("test");
                $("#startGameButton").removeAttr("hidden");
            }
            else {
                console.log("Not the owner : "+userId+" != "+gameOwnerId);
            }
        }

    });
};



// Caliing the function that get the gameState every 3 seconds
periodicGetGameState();


/*Calling the functions for the right buttons 
$("#nameGameOwner").html(username);
$("#idGameOwner").html(userId);*/
$("#gameNumber").html(code);



