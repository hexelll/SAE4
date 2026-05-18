/* -------------------------------------------- AJAX REQUESTS ------------------------------------------------ */
// Call the functions
ajaxRequests.getConnexion();


// Get the current url of the page, get the index of the "/" to not get parameters of the url and gthe actual page
let currentUrl = window.location.href.toString();
let lastIndexOfSlash = currentUrl.lastIndexOf('/');
let niceUrl = currentUrl.substring(0, lastIndexOfSlash);
//console.log("Current :" +currentUrl);
//console.log("Nice url :" +niceUrl);




/* -------------------------------------------- LOGIN  ------------------------------------------------ */
/* Fonction login for the login.html */
function login() {
    ajaxRequests.login().then(r => {
        result = r;
        
        if(result.ok) {
            //alert("Login!!!");
            let newUrl = niceUrl+"/menu.html?userId="+result.id+"&userPwd="+$("#userPwd").val()+"&username="+$("#username").val();
            //console.log(newUrl);
            window.location.replace(newUrl);

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
            let newUrl = niceUrl+"/login.html?username="+$("#username").val()+"&userPwd="+$("#userPwd").val();
            //console.log(newUrl);
            window.location.replace(newUrl);         
        }else {
            alert(result.error);
        }
    });
    
}





/* Calling the functions for the right buttons */
$("#loginButton").click(login);
$("#createAccountButton").click(createAccount);

