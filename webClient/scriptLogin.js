/* -------------------------------------------- AJAX REQUESTS ------------------------------------------------ */
// Call the functions
ajaxRequests.getConnexion();


// Get the current url of the page, get the index of the "?" to not get parameters of the url
let url = window.location.href;
let currentUrl = url.substring(0, url.indexOf('?'));




/* -------------------------------------------- LOGIN  ------------------------------------------------ */
/* Fonction login for the login.html */
function login() {
    ajaxRequests.login().then(r => {
        result = r;
        
        if(result.ok) {
            //alert("Login!!!");
            //window.location.replace("http://localhost/SAES4/SAE4/webClient/play.html?userId="+result.id+"&userPwd="+$("#userPwd").val()+"&username="+$("#username").val());
            window.location.replace(currentUrl+"/../menu.html?userId="+result.id+"&userPwd="+$("#userPwd").val()+"&username="+$("#username").val());

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
            window.location.replace(currentUrl+"/../login.html?username="+$("#username").val()+"&userPwd="+$("#userPwd").val());            
        }else {
            alert(result.error);
        }
    });
    
}





/* Calling the functions for the right buttons */
$("#loginButton").click(login);
$("#createAccountButton").click(createAccount);

