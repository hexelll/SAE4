/* -------------------------------------------- PARAMETERS ------------------------------------------------ */
// Get the parameters of the user, send from the login page
let params = new URLSearchParams(window.location.search);

let userId = params.get("userId");
let userPwd = params.get("userPwd");
let username = params.get("username");
let code = params.get("code");




// Try the connexion of debuging
//ajaxRequests.getConnexion();


// Get the current url of the page, get the index of the "/" to not get parameters of the url and gthe actual page
let currentUrl = window.location.href.toString();
let lastIndexOfSlash = currentUrl.lastIndexOf('/');
let niceUrl = currentUrl.substring(0, lastIndexOfSlash);


/* React root for the top enemy */
var nbCardsEnemyTop; 
const enemyTopRoot = ReactDOM.createRoot(document.getElementById("enemyTopHand"));

/* React root for the right enemy */
var nbCardsEnemyRight; 
const enemyRightRoot = ReactDOM.createRoot(document.getElementById("enemyRightHand"));

/* React root for the left enemy */
var nbCardsEnemyLeft; 
const enemyLeftRoot = ReactDOM.createRoot(document.getElementById("enemyLeftHand"));

/* React root for the played pile */
const playedPileRoot = ReactDOM.createRoot(document.getElementById("playedPile"));



let lastPlusCounter = 0;
let tempPlusCounter = 0;
let isMeSkipped = false;
let roots = [[enemyLeftRoot,"nameEnemyLeft", "enemyLeftHand"], [enemyTopRoot,"nameEnemyTop", "enemyTopHand"], [enemyRightRoot,"nameEnemyRight", "enemyRightHand"]];
let indexPlayer = [];




/* -------------------------------------------- ANIMATIONS ------------------------------------------------ */
function showAnimation(type, plusCounter){
    // Get elements and create animation element
    const container = document.getElementById("effectContainer");
    const animation = document.createElement("div");
    animation.className = "effectOverlay";

    const symbols = {
        "skip":  () => {
            return "<i class='bi bi-slash-circle'></i>";
        },
        "reverse": () => {
            return "<i class='bi bi-arrow-repeat'></i>";
        },
        "plus": ()=> {
            return "+"+(plusCounter == 0? lastPlusCounter+2 : plusCounter);
        },
        "pluswild": ()=> {
            return "+"+(plusCounter == 0? lastPlusCounter+4 : plusCounter);
        },
        "wild": () => {
            return "<p id='changeColor'>Change of color</p>";
        }
    };

    animation.innerHTML = symbols[type] ? symbols[type]() : "";
    container.appendChild(animation);

    // The animation last 1.5s, after it's removed
    setTimeout(() => {
        animation.remove();
    }, 1500);
}


// Creating a map to link the type of card to the symbol that will be display on it, for the actual player and the played pile
let symbolForType = {
    "normal": (value)=> {
        return value;
    },
    "skip": () => {
        return React.createElement("i", {
            className: "bi bi-slash-circle"
        });
    },
    "reverse": () => {
        return React.createElement("i", {
            className: "bi bi-arrow-repeat"
        });
    },
    "plus": (value)=> {
        return "+"+value;
    },
    "pluswild": (value)=> {
        return "+"+value;
    },
    "wild": () => {
        return React.createElement("i", {
            className: "bi bi-app-indicator"
        });
    }
}


// Triggers the skip effect on the player when a skip card is played
function triggerSkipEffect(handPlayerSkiped){
    if (handPlayerSkiped === "myHand"){
        isMeSkipped = true;
        displayCards();
        setTimeout(() => {
            isMeSkipped = false;
            displayCards();
        }, 1500);
        return;
    }

    const playerSkiped = document.getElementById(handPlayerSkiped);
    console.log("Hand skiped : " + handPlayerSkiped);

    if (!playerSkiped) return;
    playerSkiped.classList.add("playerSkipped");

    setTimeout(() => {
        playerSkiped.classList.remove("playerSkipped");
    }, 1500);
}


// Find who is the player that has been skipped and trigger the skip effect on him
function getPlayerHandElement(playerTargetedIndex, myIndex){
    console.log("Player targeted index : "+ playerTargetedIndex);
    console.log("Me index : "+ myIndex);
    let handPlayerTargeted = "";

    if (playerTargetedIndex === myIndex){
        handPlayerTargeted = "myHand";
    }
    else {
        handPlayerTargeted = indexPlayer[playerTargetedIndex][2]
    }

    console.log("Hand skiped : "+ handPlayerTargeted);
    return handPlayerTargeted;
}






/* ------------------------------------------- ACTUAL PLAYER -------------------------------------------------*/
// Creating obj json to create fake cards while wating for the server to be done
/* Width calculations and value for the overlap of all the enemies */
let myCardWidthPx = Math.min(window.innerWidth * 0.09, 200*2/3); 


/* Value for the actual player */
const myRoot = ReactDOM.createRoot(document.getElementById("myHand"));

// Get the number of cards and print it
var gameState;
var nbCardsMe;




function makeMyCards(nbCards, cards, root) {
    var myCards = [];

    console.log(cards);

    /* Create the overlap dynamicaly */
    const containerWidth = window.innerWidth*0.5; /* Container width set in vw in css */
    myCardWidthPx = Math.min(window.innerHeight * 0.24 * 2/3, 200*2/3); 
    const totalCards = nbCards;

    let overlap;
    if (totalCards > 1) {
        const step = (containerWidth - myCardWidthPx) / (totalCards - 1);

        overlap = step - myCardWidthPx;
    } else {
        overlap = 0; /* Usual overlap */
    }
    // Ensure minimum overlap of -1vw for better display
    overlap = Math.min(overlap, -1);
    console.log(overlap);
    /* Display cards */
    myCards = cards.map((card, index) =>(
        React.createElement("div", {
            key: index,
            className:  
                "cardInnerWhite " +
                (isMeSkipped ? "playerSkipped" : ""),
            style: {
                marginLeft: index === 0 ? "0vw" : overlap + "px",
                zIndex: index
            },
            onClick: () => play(card, index)
        },
            React.createElement("div", {
                className: "cardInnerColor",
                style: {
                    background: "#"+card.cardColorHex,//"linear-gradient(#"+card.cardColorHex+" 40%,rgb(from #"+card.cardColorHex+" calc(r * 0.8) calc(g * 0.7) calc(b * 1)))",
                    Index: index
                }
            },symbolForType[card.cardTypeDesc](card.cardValue))
        )
    ));

    //console.log("test : " + nbCards + " overlap : " + overlap);
    root.render(myCards);
}





/* --------------------------------------------- ENEMIES -------------------------------------------------*/
/* Width calculations and value for the overlap of all the enemies */
//const cardWidthVw = 9; /* Card width set in vw in css */
let cardWidthPx = Math.min(window.innerHeight * 0.2 * 2/3, 180*2/3); 




// Create the cards (display) 
// Different than for the actual player : because we don't know the cards the enemy has, only how many
function makeEnemysCards(nbCards, enemyRoot) {
    var enemyCards = [];

    /* Create the overlap dynamicaly */
    const containerWidth = window.innerWidth*0.3; /* Container width set in vw in css */
    cardWidthPx = Math.min(window.innerHeight * 0.2 * 2/3, 180*2/3); 
    const totalCards = nbCards;

    let overlapEnemy;
    if (totalCards > 1) {
        const step = (containerWidth - cardWidthPx) / (totalCards - 1);

        overlapEnemy = step - cardWidthPx;
    } else {
        overlapEnemy = 0; /* Usual overlap */
    }
    console.log(Math.min(overlapEnemy, -5),-cardWidthPx)
    // Ensure minimum overlap of -5vw for better display
    overlapEnemy = Math.max(Math.min(overlapEnemy, -5),-cardWidthPx);

    /* Display cards */
    for (let i = 0; i < totalCards; i++) {
        enemyCards.push(
            React.createElement("div",{
                    key:i,
                    className: "enemyCardBack",
                    style:{
                        marginLeft: i === 0 ? "0vw" : overlapEnemy + "px",
                        zIndex: i
                    }
                },
                React.createElement("div", {
                    key: i,
                    className: "enemyCard",
                    style: {
                        zIndex: i
                    }
                }, i === totalCards - 1 ? totalCards : "")
            )
        )
    }

    //console.log("test : " + nbCards + " overlap : " + overlapEnemy);
    enemyRoot.render(enemyCards);
}






/* --------------------------------------------- ACTIONS -------------------------------------------------*/
/* Create new tab from tab cards w/ function filter, the value inside the new tab are the one not played */
function play(card) {
    //console.log("Card played : " + card.cardTypeDesc);
        if (card.cardTypeDesc === "wild" || card.cardTypeDesc === "pluswild") {
            //alert("played wild or wildPlus");
            let html;
            html += "<button class='colorButton' id='redButton' onClick='chooseColor(1,"+card.cardId+")'></button>";
            html += "<button class='colorButton' id='blueButton' onClick='chooseColor(2,"+card.cardId+")'></button>";
            html += "<button class='colorButton' id='greenButton' onClick='chooseColor(3,"+card.cardId+")'></button>";
            html += "<button class='colorButton' id='yellowButton' onClick='chooseColor(4,"+card.cardId+")'></button>";

            $("#colorChoice").html(html);
        }else {
            //alert("played not a wild card");
            ajaxRequests.playCard(userId,userPwd,card.cardId).then(result => {
                // If the function on the server return ok = true, we can play the card on the client side
                if (result.ok) {
                    displayCards();
                }
                else {
                    alert(result.error);
                }
            });
            
    }
}


function chooseColor(idColor, cardId) {
    alert("color chosen : " +idColor);
    ajaxRequests.playCardWild(userId,userPwd,cardId,idColor).then(result => {
        // If the function on the server return ok = true, we can play the card on the client side
        if (result.ok) {
            displayCards();
        }
        else {
             alert(result.error);
        }
    });
    html = "";
    $("#colorChoice").html(html);
    
}



/* "Create" a new card and add it to the hand of the actual player*/
function draw() {
    ajaxRequests.drawCard(userId,userPwd).then(result => {
        /*result = JSON.parse(result);*/
        if (result.ok) {
            //console.log("Card drawn successfully");
            displayCards();
        }
        else {
            alert(result.error);
        }
    });
}



/* Scream UNO! */
function uno() {
    ajaxRequests.declareUno(userId,userPwd).then(result => {
        if (result.ok) {
            alert("UNOOOOOO!!!");
            displayCards();
        }
        else {
            alert(result.error);
        }
    });
}


/* Scream COUNTER UNO! */
function counterUno() {
    ajaxRequests.counterUno(userId,userPwd).then(result => {
        if (result.ok) {
            alert("CONTER UNO!!!");
            displayCards();
        }
        else {
            alert(result.error);
        }
    });
}




/* Calling the functions for the right buttons */
$("#drawPile").click(draw);
$("#uno").click(uno);
$("#counterUno").click(counterUno);






/* --------------------------------------------- DISPLAY CARDS -------------------------------------------------*/
/* Display the played pile */
function makePlayedPileCard(card) {
    let playedCard = React.createElement("div", {
        className: "cardInnerWhite "
        },
        React.createElement("div", {
            className: "cardInnerColor",
            style: {
                background: "#"+card.cardColorHex,//"linear-gradient(#"+card.cardColorHex+" 40%,rgb(from #"+card.cardColorHex+" calc(r * 0.8) calc(g * 0.7) calc(b * 1)))",
            }
        }, symbolForType[card.cardTypeDesc](card.cardValue))
    )
    playedPileRoot.render(playedCard);
}



/* To display the result of getGameState every 3 seconds */
function periodicGetGameState(){
    displayCards();
    setTimeout(periodicGetGameState, 3000); 
}




window.addEventListener("resize", displayCards);
window.addEventListener("load", displayCards);


let lastCurrentCard;
let temp;

let lastCurrentPlayerIndex = 0;
let tempCurrentPlayer = 0;


async function displayCards() {
    //console.log(userId,userPwd);
    ajaxRequests.getGameState(userId,userPwd).then(g => {
        gameState = g;
        console.log(gameState);

        // If the game is started
        if (gameState.isStarted == true) {
            lastPlusCounter = tempPlusCounter;
            tempPlusCounter = gameState.plusCounter;

            let indexRoot = 0;
            let indexMe = 0;

            let currentPlayerIndicator = "<p style=\"color:#0ff35b\">\u2b24</p>";

            for (let i = 0; i < gameState.players.length; i++) {
                // To get the place in the list of the actual player
                if (parseInt(userId) === gameState.players[i].playerId) {
                    nbCardsMe = gameState.cards.length; 
                    makeMyCards(nbCardsMe, gameState.cards, myRoot);
                    let name = "Player "+ (i+1) + " : " + gameState.players[i].username;
                    indexMe = i;

                    // Check if it's this player's turn
                    if (gameState.currentPlayerIndex === i) {
                        name += " " + currentPlayerIndicator;
                    }
                    $("#nameMe").html(name);
                    $("#nbCardsMe").html("Number of cards : "+nbCardsMe);
                    //indexPlayer.push(i);
                    break;
                }
            }
            indexPlayer = []
            for (let i = (indexMe+1)%gameState.players.length; i!=indexMe; i=(i+1)%gameState.players.length) {
                makeEnemysCards(gameState.players[i].cardCount, roots[indexRoot][0]);
                let name = "Player "+ (i+1) + " : " +gameState.players[i].username;
                // Check if it's this player's turn
                if (gameState.currentPlayerIndex === i) {
                    name += " " + currentPlayerIndicator;
                }
                $("#"+roots[indexRoot][1]).html(name);
                indexPlayer[i] = roots[indexRoot]

                indexRoot++;
            }

            // Get the last and the current card 
            lastCurrentCard = temp;
            temp = gameState.currentCard;

            // If the 2 are different, it means somebody has played a card, so we can display the animation of the card
            if (lastCurrentCard.cardId != gameState.currentCard.cardId) {
                showAnimation(gameState.currentCard.cardTypeDesc, gameState.plusCounter);

                // If the card played is a skip, it triggers the skip effect on the next player
                if (gameState.currentCard.cardTypeDesc === "skip"){
                    let skippedPlayerIndex;
                    if (gameState.isReversed === false){
                        skippedPlayerIndex = (gameState.currentPlayerIndex - 1) % gameState.players.length;
                        skippedPlayerIndex = skippedPlayerIndex < 0 ? skippedPlayerIndex + gameState.players.length : skippedPlayerIndex;
                    }
                    else {
                        skippedPlayerIndex = (gameState.currentPlayerIndex+1) % gameState.players.length;
                        skippedPlayerIndex = skippedPlayerIndex < 0 ? skippedPlayerIndex + gameState.players.length : skippedPlayerIndex;
                    }
                    console.log("Skiped player index : "+ skippedPlayerIndex);

                    let skippedHand = getPlayerHandElement(skippedPlayerIndex, indexMe);
                    triggerSkipEffect(skippedHand);
                }

                if ((gameState.currentCard.cardTypeDesc === "plus" || gameState.currentCard.cardTypeDesc === "pluswild") && lastPlusCounter >= 0 && gameState.plusCounter === 0) {
                    console.log("Current card played : " + gameState.currentCard.cardTypeDesc);
                    console.log("Plus counter : " + gameState.plusCounter);
                    console.log("Last Plus counter : " +lastPlusCounter);

                    let skippedPlayerIndex;
                    if (gameState.isReversed === false){
                        skippedPlayerIndex = (gameState.currentPlayerIndex-1) % gameState.players.length;
                        skippedPlayerIndex = skippedPlayerIndex < 0 ? skippedPlayerIndex + gameState.players.length : skippedPlayerIndex;
                    }
                    else {
                        skippedPlayerIndex = (gameState.currentPlayerIndex+1) % gameState.players.length;
                        skippedPlayerIndex = skippedPlayerIndex < 0 ? skippedPlayerIndex + gameState.players.length : skippedPlayerIndex;
                    }
                    console.log("Skiped player index : "+ skippedPlayerIndex);

                    let skippedHand = getPlayerHandElement(skippedPlayerIndex, indexMe);
                    triggerSkipEffect(skippedHand);
                    
                }

            }


            



            // Display the last played card on the played cards pile
            makePlayedPileCard(gameState.currentCard);
        }

        // If the game is not started, it means it ended
        else {
            lastCurrentPlayerIndex = tempCurrentPlayer;
            tempCurrentPlayer = gameState.currentPlayerIndex;

            for(let i = 0; i < gameState.players.length; i++) {
                winner = lastCurrentPlayerIndex;
                alert("Game ended! Winner : " + winner);

                let newUrl = niceUrl+"/lobby.html?userId="+userId+"&userPwd="+userPwd+"&username="+username+"&code="+code;
                console.log(newUrl);
                window.location.replace(newUrl);
            }
        
        }
                
    });
};


// Caliing the function that get the gameState every 3 seconds
periodicGetGameState();

