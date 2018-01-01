<?php

require_once("functions.php");

function err($message){

    header("Location: login.php?error=" . $message);
}
session_start();
$_SESSION['admin'] = false;
$login = $_POST["login"];
$pass = $_POST["passwd"];
if($login==""){
    err("emptylogin");
} else if($pass==""){
    err("emptypass");
} else if(strlen($login)<5 || strlen($login)>20){
    err("loginlength");
} else if(strlen($pass)<8 || strlen($pass)>40){
    err("passlength");
} else {
    $post_data = http_build_query(
        array(
            'secret' => "6Le3mT4UAAAAAMUlSezcehIEXHai9oDBfVpwiWPB",
            'response' => $_POST['g-recaptcha-response'],
            'remoteip' => $_SERVER['REMOTE_ADDR']
        )
    );
    $opts = array('http' =>
        array(
            'method'  => 'POST',
            'header'  => 'Content-type: application/x-www-form-urlencoded',
            'content' => $post_data
        )
    );
    $context  = stream_context_create($opts);
    $response = file_get_contents('https://www.google.com/recaptcha/api/siteverify', false, $context);
    $result = json_decode($response);
    if (!$result->success) {
        err("kapcza");
    } else {
        $con = mysqli_connect("localhost","root","zaq1@WSX","bank");

        if (mysqli_connect_errno()){
            echo "Failed to connect to MySQL: " . mysqli_connect_error();
        } else {
            $stmt = ("SELECT passwd, admin FROM users WHERE login='".$login."';");
            //$stmt->bind_param('s',$login);
            //$stmt->execute();
            //echo $stmt;
            $result = mysqli_query($con,$stmt);
            if($result){
                if($row=$result->fetch_object()){
                    if(checkpassword($pass,$row->passwd)){
                        if($row->admin){
                            $_SESSION['admin'] = true;
                        }
                        $_SESSION['userlogged'] = $login;
                        header("Location: login.php");
                    } else {
                        err("wrongdata");
                    }

                } else {
                    err("wrongdata");
                }
            }
        }
    }

}

?>