/* -------------------------------------------- AJAX REQUESTS ------------------------------------------------ */
// Call the functions
ajaxRequests.getConnexion();



/* -------------------------------------------- LOGIN  ------------------------------------------------ */
/* Fonction login for the login.html */
function login() {
    ajaxRequests.login().then(r => {
        result = r;
        if(result.ok) {
            alert("Login!!!");
            window.location.replace("http://localhost/SAES4/SAE4/webClient/play.html?userId="+result.id+"&userPwd="+$("#userPwd").val());
            
        }else {
            alert(result.error);
        }
    });
    
}




/* Calling the functions for the right buttons */
$("#loginButton").click(login);
