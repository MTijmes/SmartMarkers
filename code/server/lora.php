<?php
    include 'functions.php';
    $mysqli = connectDatabase();


	$json_a = json_decode(file_get_contents('php://input'), true);
	echo "Test!";

    $sql = 'INSERT INTO logging (device, payload) VALUES (?, ?)';
        $stmt = $mysqli -> prepare($sql);
        $stmt -> bind_param('ss', $json_a['DevEUI_uplink']['DevEUI'],$json_a['DevEUI_uplink']['payload_hex']);
        $stmt -> execute();

?>
