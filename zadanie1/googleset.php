
<?php

session_start();
function err($message){

    header("Location: wszystkieprzelewy.php?error=" . $message);
}
$login = $_SESSION['userlogged'];
$id = $_GET['id'];



    $con = mysqli_connect("localhost","root","zaq1@WSX","bank");

    if (mysqli_connect_errno()){
        echo "Failed to connect to MySQL: " . mysqli_connect_error();
    } else {
                $stmt2 = $con->prepare("UPDATE users SET googleid='".$id."' where login = '".$_SESSION['userlogged']."';");
                if($stmt2){
                    //$stmt2->bind_param('sssssd',$login,$name,$acc,$addr,$title,$amount);
                    $stmt2->execute();
                    if($stmt2){
                            header("Location: google.php");

                        //echo "gut";
                    } else {
                            err();

                    }

                }

    }


?>
