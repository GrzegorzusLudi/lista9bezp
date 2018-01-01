<!DOCTYPE html>
<html>
<head>
<meta name="google-signin-client_id" content="88825449054-mkvt8g4hbj7s2t9bb344ev1lag564ved.apps.googleusercontent.com">
<style>
body{background-color:black;color:white;text-align:center}
a{text-decoration:none;color:#ccf}
#zdiv{border:1px solid white;width:400px;margin:auto;margin-top:10px;background-color:#222}
#titl{border:1px solid white;width:600px;font-size:30pt;margin:auto;margin-top:10px;background-color:#222}
#links{font-size:14pt;margin-top:5px}
</style>
<script src="https://apis.google.com/js/platform.js" async defer></script>
<script src='https://www.google.com/recaptcha/api.js'></script>
<script>
    <?php
        if(!isset($_SESSION['userlogged'])){
            echo "first=1;";
        }
    ?>
</script>
</head>
<body>
<div id="titl">
Twój bank
<div id="links">
<?php
session_start();
if(isset($_SESSION['userlogged'])){
    header("Location: przelew.php");
} else {
    echo(file_get_contents('./unlogged.txt'));
}
?>
</div>
</div>
<div id="zdiv">

<p>
Zaloguj:
</p>
<form method="POST" action="logintodatabase.php">
Numer konta: <input type="text" name="login"/><br>
Hasło: <input type="password" name="passwd"/><br>

<div style="color:red">
<?php
if(empty($_GET["error"])){
    echo "";
}else if($_GET["error"]=="emptylogin") {
    echo "Pusty login!";
}else if($_GET["error"]=="emptypass") {
    echo "Puste hasło!";
}else if($_GET["error"]=="loginlength") {
    echo "Login powinien mieć od 5 do 20 znaków!";
}else if($_GET["error"]=="passlength") {
    echo "Hasło powinien mieć od 8 do 40 znaków!";
}else if($_GET["error"]=="wrongdata") {
    echo "Login lub hasło niepoprawne";
}else if($_GET["error"]=="kapcza") {
    echo "Chyba jednak jesteś robotem.";
}
?>
</div>
<input type="submit" value="Loguj" />
<div class="g-recaptcha" data-sitekey="6Le3mT4UAAAAAMxLeIGjYffxV2XGQErUV8pDSF7K"></div>
</form>
<p>
<br/>Zaloguj przez konto Google:
<div style="text-align:center" class="g-signin2" data-onsuccess="onSignIn"></div>
<!--a href="#" onclick="signOut();">Sign out</a-->
<form id="s" method="POST" action="googlelogin.php">
<input type="hidden" name="id" id="ii" value="" />
<input type="submit" value="Loguj" style="display:none" />
<div class="g-recaptcha" data-sitekey="6Le3mT4UAAAAAMxLeIGjYffxV2XGQErUV8pDSF7K"></div>
</form>
<script>
  function onSignIn(googleUser) {
    <?php
        if(!isset($_SESSION['userlogged'])){
            echo "if(first==1){signOut();}";
        }
    ?>
    if(first==0){
        var id_token = googleUser.getAuthResponse().id_token;
        document.getElementById("ii").value = id_token.substr(0,200);
        document.getElementById("s").submit();
    }
    first = 0;
    /*
    alert('ID: ' + profile.getId());
  alert('Full Name: ' + profile.getName());
  alert('Given Name: ' + profile.getGivenName());
  alert('Family Name: ' + profile.getFamilyName());
  alert('Image URL: ' + profile.getImageUrl());
  alert('Email: ' + profile.getEmail());

*/
  }
  function signOut() {
    var auth2 = gapi.auth2.getAuthInstance();
    auth2.signOut().then(function () {
    });
  }
</script>

</div>
</body>
</html>
