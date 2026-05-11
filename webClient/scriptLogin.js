/* -------------------------------------------- AJAX REQUESTS ------------------------------------------------ */
// Call the functions
ajaxRequests.getConnexion();



/* -------------------------------------------- LOGIN  ------------------------------------------------ */
/* Fonction login for the login.html */
function login() {
    ajaxRequests.login().then(r => {
        result = r;
        if(result.ok) {
            //alert("Login!!!");
            //window.location.replace("http://localhost/SAES4/SAE4/webClient/play.html?userId="+result.id+"&userPwd="+$("#userPwd").val()+"&username="+$("#username").val());
            window.location.replace("http://localhost/SAES4/SAE4/webClient/menu.html?userId="+result.id+"&userPwd="+$("#userPwd").val()+"&username="+$("#username").val());

        }else {
            alert(result.error);
        }
    });
    
}



/* -------------------------------------------- CREATE ACCOUNT  ------------------------------------------------ */
/* Fonction login for the login.html */
function createAccount() {
    ajaxRequests.createAccount().then(r => {
        result = r;
        if(result.ok) {
            //alert("Create Account!!!");
            window.location.replace("http://localhost/SAES4/SAE4/webClient/login.html?username="+$("#username").val()+"&userPwd="+$("#userPwd").val());            
        }else {
            alert(result.error);
        }
    });
    
}





/* Calling the functions for the right buttons */
$("#loginButton").click(login);
$("#createAccountButton").click(createAccount);

