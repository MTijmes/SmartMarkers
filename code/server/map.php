<!DOCTYPE html>
<html>
  <head>
    <style>
      #map {
        height: 800px;
        width: 100%;
       }
    </style>
    <title>
      Meetpunten
    </title>
  </head>
  <body>
    <div id="map"></div>
    <script>
      function initMap() {
        var uluru = {lat: 52.501153, lng: 6.0780437};
        var map = new google.maps.Map(document.getElementById('map'), {
          zoom: 15,
          center: uluru
        });
        var marker = new google.maps.Marker({
          position: uluru,
          map: map
        });

        var ctaLayer = new google.maps.KmlLayer({
          url: 'https://4g.deboterkamp.nl/kml.php?t=<?php echo time();?>',
          map: map
        });

      }

    </script>
    <script async defer
    src="https://maps.googleapis.com/maps/api/js?key=AIzaSyB2rwXdolY6la-Uq35ApHGzkX8jpUVt94c&callback=initMap">
    </script>
  </body>
</html>
