<?php

  require("functions.php");

  // Opens a connection to a MySQL server
  $mysqli = connectDatabase();

  $sql = 'SELECT * FROM measurements';
  $stmt = $mysqli->prepare($sql);
  $stmt -> execute();
  $stmt -> bind_result($res['id'], $res['marker'], $res['lat'], $res['lon'], $res['timestamp']);

  $kml = array('<?xml version="1.0" encoding="UTF-8"?>');
  $kml[] = '<kml xmlns="http://earth.google.com/kml/2.2">';
  $kml[] = "\t" . '<Document>';
  $kml[] = "\t\t" . '<name>KmlFile</name>';
  $kml[] = "\t\t" . '<Style id="west_campus_style">';
  $kml[] = "\t\t" . '<IconStyle>';
  $kml[] = "\t\t\t" . '<Icon>';
  $kml[] = "\t\t\t\t" . '<href>https://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>';
  $kml[] = "\t\t\t" . '</Icon>';
  $kml[] = "\t\t" . '</IconStyle>';
  $kml[] = "\t\t" . '<BalloonStyle>';
  $kml[] = "\t\t\t" . '<text>$[info]</text>';
  $kml[] = "\t\t" . '</BalloonStyle>';
  $kml[] = "\t\t" . '</Style>';

  // Iterate for each point
  while ($stmt->fetch()){
    $kml[] = "\t\t" . '<Placemark>';
    $kml[] = "\t\t\t" . '<name>';
    $kml[] = "\t\t\t\t" . $res['id'];
    $kml[] = "\t\t\t" . '</name>';
    $kml[] = "\t\t\t" . '<styleUrl>';
    $kml[] = "\t\t\t\t" . '#west_campus_style';
    $kml[] = "\t\t\t" . '</styleUrl>';
    $kml[] = "\t\t\t" . '<description>';
    $kml[] = "\t\t\t\t\t\t" . 'Meetpunt: ' . $res['id'] . '&lt;/br&gt;';
    $kml[] = "\t\t\t\t\t\t" . 'Tijdstip: ' . $res['timestamp'] . '&lt;/br&gt;';
    $kml[] = "\t\t\t\t\t\t" . 'Locatie: ' . $res['lat'] . ', ' . $res['lon'] . '&lt;/br&gt;';
    $kml[] = "\t\t\t" . '</description>';
    $kml[] = "\t\t\t" . '<Point>';
    $kml[] = "\t\t\t\t" . '<coordinates>';
    $kml[] = "\t\t\t\t\t" . $res['lon'] . ',' . $res['lat'] . ',0';
    $kml[] = "\t\t\t\t" . '</coordinates>';
    $kml[] = "\t\t\t" . '</Point>';
    $kml[] = "\t\t" . '</Placemark>';
  }

  $kml[] = "\t" . '</Document>';
  $kml[] = '</kml>';

  unset ($res);

  $kmlOutput = join("\n", $kml);
  header('Content-type: application/vnd.google-earth.kml+xml');
  echo $kmlOutput;
?>
