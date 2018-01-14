<?php

  require("functions.php");

  // Opens a connection to a MySQL server
  $mysqli = connectDatabase();

  // Get lots from database
  $sql = 'SELECT lots.id, lots.name, lots.created, link_table.measurement, measurements.lon, measurements.lat FROM lots
          JOIN link_table ON link_table.lot = lots.id
          JOIN measurements ON link_table.measurement = measurements.id
          ORDER BY lots.id, link_table.id';
  $stmt = $mysqli->prepare($sql);
  $stmt -> execute();
  $stmt -> bind_result($res['id'], $res['name'], $res['created'], $res['measurement'], $res['lon'], $res['lat']);

  $kml = array('<?xml version="1.0" encoding="UTF-8"?>');
  $kml[] = '<kml xmlns="http://earth.google.com/kml/2.2">';
  $kml[] = "\t" . '<Document>';
  $kml[] = "\t\t" . '<name>KmlFile</name>';

  $lastID = 0;
  while ($stmt->fetch()){
    if($lastID != $res['id'] && $lastID != 0){
      $kml[] = "\t\t\t\t\t\t" . '</coordinates>';
      $kml[] = "\t\t\t\t\t" . '</LinearRing>';
      $kml[] = "\t\t\t\t" . '</outerBoundaryIs>';
      $kml[] = "\t\t\t" . '</Polygon>';
      $kml[] = "\t\t" . '</Placemark>';
    }
    if($lastID != $res['id']){
      $lastID = $res['id'];

      $kml[] = "\t\t" . '<Placemark>';
      $kml[] = "\t\t\t" . '<description>';
      $kml[] = "\t\t\t\t" . 'Perceel: ' . $res['name'] . '&lt;br&gt;';
      $kml[] = "\t\t\t\t" . 'Created: ' . $res['created'];
      $kml[] = "\t\t\t" . '</description>';
      $kml[] = "\t\t\t" . '<Polygon>';
      $kml[] = "\t\t\t\t" . '<outerBoundaryIs>';
      $kml[] = "\t\t\t\t\t" . '<LinearRing>';
      $kml[] = "\t\t\t\t\t\t" . '<coordinates>';
    }

    $kml[] = "\t\t\t\t\t\t\t" . $res['lon'] . ',' . $res['lat'] . ',0';
  }

$kml[] = "\t\t\t\t\t\t" . '</coordinates>';
      $kml[] = "\t\t\t\t\t" . '</LinearRing>';
      $kml[] = "\t\t\t\t" . '</outerBoundaryIs>';
      $kml[] = "\t\t\t" . '</Polygon>';
      $kml[] = "\t\t" . '</Placemark>';

  $kml[] = "\t" . '</Document>';
  $kml[] = '</kml>';
unset ($res);

  $kmlOutput = join("\n", $kml);
  header('Content-type: application/vnd.google-earth.kml+xml');
  echo $kmlOutput;
?>
