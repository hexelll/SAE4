/* -------------------------------------------- AJAX REQUESTS ------------------------------------------------ */
//import ajaxRequests from "./ajaxRequests.js";



// Call the functions
ajaxRequests.getConnexion();

/* ------------------------------------------- ACTUAL PLAYER -------------------------------------------------*/
// Creating obj json to create fake cards while wating for the server to be done
/* Width calculations and value for the overlap of all the enemies */
//const myCardWidthVw = 9; /* Card width set in vw in css */
const myCardWidthPx = Math.min(window.innerWidth * 0.09, 120); 
const myCardWidthVw = (myCardWidthPx / window.innerWidth) * 100;


/* Value for the actual player */
const myRoot = ReactDOM.createRoot(document.getElementById("myHand"));

/*
var cards = [
    { value: 1, color: "red" },
    { value: 5, color: "blue" },
    { value: 9, color: "green" },
    { value: "+ 4", color: "black" },
];*/
// Get the number of cards and print it
var gameState;
var nbCardsMe;



function organizeCards(cardsToOrganize) {
    cardsToOrganize.sort((a,b) => {
        const colorOrder = ["red", "blue", "green", "yellow", "black"];
        if (a.color === b.color) {
            return a.value-b.value;
        }
        return colorOrder.indexOf(a.color) - colorOrder.indexOf(b.color);
    }
    );
    console.log("sorted cards:",cardsToOrganize);
}



function makeMyCards(nbCards, cards, root) {
    var myCards = [];

    console.log(cards);

    /* Create the overlap dynamicaly */
    const containerWidthVw = 40; /* Container width set in vw in css */
    const totalCards = nbCards;

    let overlap;
    if (totalCards > 1) {
        const stepVw = (containerWidthVw - myCardWidthVw) / (totalCards - 1);

        overlap = stepVw - myCardWidthVw;
    } else {
        overlap = 0; /* Usual overlap */
    }

    // Ensure minimum overlap of -1vw for better display
    overlap = Math.min(overlap, -1);
    /* Display cards */
    myCards = cards.map((card, index) =>(
        React.createElement("div", {
            key: index,
            className: "cardInnerWhite",
            style: {
                marginLeft: index === 0 ? "0vw" : overlap + "vw",
                zIndex: index
            },
            onClick: () => play(card, index)
        },
            React.createElement("div", {
                className: "cardInnerColor",
                style: {
                    backgroundColor: "#"+card.cardColorHex,
                    Index: index
                }
            }, card.cardTypeDesc == "plus" ? "+"+card.cardValue : card.cardValue +"")
        )
    ));

    console.log("test : " + nbCards + " overlap : " + overlap);
    root.render(myCards);
}



// Create the cards (print)
function makeCards() {
    var myCards = cards.map((card, index) =>
        React.createElement("div", {
            key: index,
            className: "cardInnerWhite",
            onClick: () => play(card, index)
        },
            React.createElement("div", {
                className: "cardInnerColor",
                style: {
                    backgroundColor: card.color,
                    Index: index
                }
            }, card.value)
        )
    );
    myRoot.render(myCards);
    console.log(myCards);
}




/* --------------------------------------------- ENEMIES -------------------------------------------------*/
/* Width calculations and value for the overlap of all the enemies */
//const cardWidthVw = 9; /* Card width set in vw in css */
const cardWidthPx = Math.min(window.innerWidth * 0.09, 120); 
const cardWidthVw = (cardWidthPx / window.innerWidth) * 100;


/* Value for the top enemy */
var nbCardsEnemyTop; 
const enemyTopRoot = ReactDOM.createRoot(document.getElementById("enemyTopHand"));

/* Values  for the right enemy */
var nbCardsEnemyRight; 
const enemyRightRoot = ReactDOM.createRoot(document.getElementById("enemyRightHand"));


/* Values for the left enemy */
var nbCardsEnemyLeft; 
const enemyLeftRoot = ReactDOM.createRoot(document.getElementById("enemyLeftHand"));



/* ---------------------------------- Enemy's hand -----------------------------------*/
// Create the cards (display) 
// Different than for the actual player : because we don't know the cards the enemy has, only how many
function makeEnemysCards(nbCards, enemyRoot) {
    var enemyCards = [];

    /* Create the overlap dynamicaly */
    const containerWidthVw = 40; /* Container width set in vw in css */
    const totalCards = nbCards;

    let overlapEnemy;
    if (totalCards > 1) {
        const stepVw = (containerWidthVw - cardWidthVw) / (totalCards - 1);

        overlapEnemy = stepVw - cardWidthVw;
    } else {
        overlapEnemy = 0; /* Usual overlap */
    }

    // Ensure minimum overlap of -5vw for better display
    overlapEnemy = Math.min(overlapEnemy, -5);

    /* Display cards */
    for (let i = 0; i < totalCards; i++) {
        enemyCards.push(
            React.createElement("div", {
                key: i,
                className: "enemyCard",
                style: {
                    marginLeft: i === 0 ? "0vw" : overlapEnemy + "vw",
                    zIndex: i
                }
            }, i === totalCards - 1 ? totalCards : "")
        )
    }

    //console.log("test : " + nbCards + " overlap : " + overlapEnemy);
    enemyRoot.render(enemyCards);
}






/* --------------------------------------------- ACTIONS -------------------------------------------------*/
/* Create new tab from tab cards w/ function filter, the value inside the new tab are the one not played */
function play(card) {
    ajaxRequests.playCard(card.cardId).then(result => {
        /*result = JSON.parse(result);*/
        if (result.ok) {
            console.log("Card played successfully");
            ajaxRequests.getGameState().then(g => {
                gameState = g;
                nbCardsMe = gameState.cards.length;
                makeMyCards(nbCardsMe,gameState.cards, myRoot);

                if (nbCardsMe === 1) {
                    $("#uno").removeAttr("hidden");
                }
            });
        }
        else {
            alert(result.error);
        }
    });
}


/* "Create" a new card and add it to the hand of the actual player*/
function draw() {
    var colors = ["red", "blue", "green", "yellow"];
    var values = [1, 2, 3, 4, 5, 6, 7, 8, 9, "+2", "change"];
    var randomColor = Math.floor(Math.random() * colors.length);
    var randomValue = Math.floor(Math.random() * values.length);
    console.log("Random color : " + colors[randomColor]);
    console.log("Random value : " + values[randomValue]);

    var newCard = { value: values[randomValue], color: colors[randomColor] };
    cards.push(newCard);
    organizeCards(cards);

    //console.log("I draw a card", cards);
    nbCardsMe += 1;
    if (nbCardsMe > 1) {
        $("#uno").attr("hidden", true);
    }
    makeMyCards(nbCardsMe, myRoot);
}


/* Scream UNO! */
function uno() {
    ajaxRequests.getGameState().then(g => {
        gameState = g;
        nbCardsMe = gameState.cards.length;

        makeMyCards(nbCardsMe, gameState.cards, myRoot);

        $("playedPile").html(gameState.currentCard.cardValue + " " + gameState.currentCard.cardColorHex);
    });
    alert("UNOOOOOO!!!");
}



window.addEventListener("resize", displayCards);
window.addEventListener("load", displayCards);

function displayCards() {
    ajaxRequests.getGameState().then(g => {
        gameState = g;

        // Get the number of cards
        nbCardsMe = gameState.cards.length;
        nbCardsEnemyTop = gameState.players[0].cardCount;
        nbCardsEnemyRight = gameState.players[1].cardCount;
        nbCardsEnemyLeft = gameState.players[2].cardCount;
        

        //Call functions of display
        makeEnemysCards(nbCardsEnemyTop, enemyTopRoot);
        makeEnemysCards(nbCardsEnemyRight, enemyRightRoot);
        makeEnemysCards(nbCardsEnemyLeft, enemyLeftRoot);
        makeMyCards(nbCardsMe, gameState.cards, myRoot);

        $("#playedPile").html(gameState.currentCard.cardValue + " " + gameState.currentCard.cardColorHex);
    });
};



$("#drawPile").click(draw);
$("#uno").click(uno);

