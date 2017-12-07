<?php

    function connectDatabase(){
        $mysqli = new mysqli('localhost', 'gebruiker', 'W8woord!', 'smart_marker');
        //$mysqli = new mysqli($config['mysql']['hostname'], $config['mysql']['username'], $config['mysql']['password'], $config['mysql']['database']);
        return $mysqli;

    }

    function disconnectDatabase(){
        global $mysqli;
        $mysqli->close();
    }

    function dateOnly($date){
        $date = date_create($date);
        return date_format($date, 'd-m-Y');
    }

    function getDateStamp(){
        $date = date_create(getTimeStamp());
        return date_format($date, 'Y-m-d');
    }


    function getTimeStamp(){
        $date = date_create();
        return date_format($date, 'Y-m-d H:i:s');
    }

?>
