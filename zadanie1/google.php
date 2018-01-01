<!DOCTYPE html>
<html>
<head>
<meta name="google-signin-client_id" content="88825449054-mkvt8g4hbj7s2t9bb344ev1lag564ved.apps.googleusercontent.com">
<style>
body{background-color:black;color:white;text-align:center}
a{text-decoration:none;color:#ccf}
#zdiv{border:1px solid white;width:600px;margin:auto;margin-top:10px;background-color:#222}
#titl{border:1px solid white;width:600px;font-size:30pt;margin:auto;margin-top:10px;background-color:#222}
#links{font-size:14pt;margin-top:5px}
</style>
<script src="https://apis.google.com/js/platform.js" async defer></script>
</head>
<body onload="logged = false;">
<div id="titl">
Twój bank
<div id="links">
<?php
session_start();
if(isset($_SESSION['userlogged'])){
    $login = $_SESSION['userlogged'];
    $adm = "";
    $atm = "";
    if($_SESSION['admin']){
        $adm = " <span style=\"color:red\">[ADMIN]</span>";
        $atm = file_get_contents('adminpanel.txt');
    }
    echo("Zalogowany: ".$_SESSION['userlogged'] . $adm . '<br/>' . file_get_contents('logged.txt') . $atm);
} else {
    header("Location: login.php");
}
?>
</div>
</div>
<div id="zdiv">
Zaloguj się przez Google, by połączyć swoje konto bankowe z kontem Google:
<div style="text-align:center" class="g-signin2" data-onsuccess="onSignIn" onclick="logged=true;"></div>
<div id="data"></div>
<a id="so" href="#" onclick="signOut();"></a>
<script>
  function onSignIn(googleUser) {
    if(logged){
    var id_token = googleUser.getAuthResponse().id_token.substr(0,200);
    window.location = 'googleset.php?id='+id_token;
    } else {
    var profile = googleUser.getBasicProfile();
        document.getElementById("data").innerHTML="<img src=\""+profile.getImageUrl()+"\" /> <h3>"+profile.getName()+"</h3>";
        document.getElementById("so").innerHTML="Wyloguj się";
    }
  }

  function signOut() {
    var auth2 = gapi.auth2.getAuthInstance();
    auth2.signOut().then(function () {
        location.reload();
    });
  }
</script>
</div>
</body>
</html>
