<?php
require("functions.php");

  if(isset($_GET['trunc'])){                        // Remove all existing lots from DB
    $mysqli = connectDatabase();

    $sql = 'TRUNCATE lots;';
    $stmt = $mysqli -> prepare($sql);
    $stmt -> execute();

    $sql = 'TRUNCATE link_table;';
    $stmt = $mysqli -> prepare($sql);
    $stmt -> execute();
  }

  if(isset($_POST['delete'])){                        // Remove selected lot
    $mysqli = connectDatabase();

    $sql = 'DELETE FROM lots WHERE id = (?);';
    $stmt = $mysqli -> prepare($sql);
    $stmt -> bind_param('i', $_POST['delete']);
    $stmt -> execute();

    $sql = 'DELETE FROM link_table WHERE lot = (?);';
    $stmt = $mysqli -> prepare($sql);
    $stmt -> bind_param('i', $_POST['delete']);
    $stmt -> execute();
  }

  if (isset($_POST['myInputs'])){                     // Make new lot from selected points

    $mysqli = connectDatabase();

    $sql = 'INSERT INTO lots (name) VALUES (?);';
    $stmt = $mysqli -> prepare($sql);
    $stmt -> bind_param('s', $_POST['name']);
    $stmt -> execute();

    $sql = 'SELECT LAST_INSERT_ID() FROM lots;';      // Get ID from inserted lot
    $stmt = $mysqli -> prepare($sql);
    $stmt -> execute();
    $last_id = $stmt -> insert_id;

    foreach($_POST['myInputs'] as $id){               // Add measurements to inserted lot
      $mysqli = connectDatabase();
      $sql = 'INSERT INTO link_table (lot, measurement) VALUES (?, ?);';
      $stmt = $mysqli -> prepare($sql);
      $stmt -> bind_param('ii', $last_id, $id);
      $stmt -> execute();
    }


  }
  ?>

  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="initial-scale=1.0">
    <meta charset="utf-8">
    <title>KBS ESA3 | Map</title>
    <style>
    html, body {
      height: 100%;
      margin: 0;
      padding: 0;
    }
    #map {
      float: left;
      height: 100%;
      width: 79%;
    }
    #content-window {
      float: left;
      font-family: 'Roboto','sans-serif';
      height: 50%;
      line-height: 30px;
      padding-left: 10px;
      width: 19%;
    }
  </style>
</head>
<body>
  <div id="map"></div>
  <div id="content-window"></div>



  <script>
    function initMap() {
      var uluru = {lat: 52.501153, lng: 6.0780437};
      var map = new google.maps.Map(document.getElementById('map'), {
        zoom: 15,
        center: uluru
      });

      var polygonLayer = new google.maps.KmlLayer({                             // Add layer with lots
        url: 'https://thuis.deboterkamp.nl/polygon.php?t=<?php echo time();?>', // Echo time to force a not cached KML file
        suppressInfoWindows: true,
        map: map
      })

      var kmlLayer = new google.maps.KmlLayer({                                 // Add layer with measurements
        url: 'https://thuis.deboterkamp.nl/kml.php?t=<?php echo time();?>',
        suppressInfoWindows: true,
        map: map
      });

      polygonLayer.addListener('click', function(kmlEvent) {
        var text = '<form method="POST"><button type="submit" name="delete" value="' + kmlEvent.featureData.name + '">Verwijder perceel</button></form><br>';
        text += kmlEvent.featureData.description;
        showInContentWindow(text);
      });

      kmlLayer.addListener('click', function(kmlEvent) {
        var text = '<button onclick="addInput(\'dynamicInput\', ' + kmlEvent.featureData.name + ')">Voeg toe aan perceel</button><br>';
        text += kmlEvent.featureData.description;
        showInContentWindow(text);
      });

      function showInContentWindow(text) {
        var sidediv = document.getElementById('content-window');
        sidediv.innerHTML = text;
      }
    }

    function addInput(divName,marker){    // Add new marker to selection

      var newdiv = document.createElement('div');

      newdiv.innerHTML = "Meetpunt " + marker + "<input type='hidden' value='" + marker + "' name='myInputs[]' readonly>";

      document.getElementById(divName).appendChild(newdiv);
    }

  </script>
  <script async defer
  src="https://maps.googleapis.com/maps/api/js?key=AIzaSyB2rwXdolY6la-Uq35ApHGzkX8jpUVt94c&callback=initMap">
  </script>

  <form method="POST">
    <input type="text" placeholder="Naam" name="name">
    <div id="dynamicInput">

     Meetpunten:

    </div>

    <input type="submit" value="Maak perceel">

  </form>

</body>
</html>
