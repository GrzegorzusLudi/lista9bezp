<?php

require_once("functions.php");
require_once 'gaphp/vendor/autoload.php';

function err($message){

    header("Location: login.php?error=" . $message);
}
session_start();
$_SESSION['admin'] = false;

$id = substr($_POST['id'],0,200);
if($id==""){
    err("emptyid");
} else {
    /*$CLIENT_ID = "88825449054-mkvt8g4hbj7s2t9bb344ev1lag564ved.apps.googleusercontent.com";
    $client = new Google_Client(['client_id' => $CLIENT_ID]);
    $payload = $client->verifyIdToken($id);

    if ($payload) {
        $userid = $payload['sub'];
        echo $userid;
    } else {
        echo "lol";
    }*/
    $con = mysqli_connect("localhost","root","zaq1@WSX","bank");
    if (mysqli_connect_errno()){
        echo "Failed to connect to MySQL: " . mysqli_connect_error();
    } else {
        $stmt = ("SELECT login,admin FROM users WHERE googleid='".$id."';");
        //$stmt->execute();
        //echo "lol";
        $result = mysqli_query($con,$stmt);
        if($result){
            if($row=$result->fetch_object()){
                if($row->admin){
                    $_SESSION['admin'] = true;
                }
                $_SESSION['userlogged'] = $row->login;
                header("Location: przelew.php");

            } else {
                //echo $id;
                err("wrongdata");
            }
        }
    }

}

?>
