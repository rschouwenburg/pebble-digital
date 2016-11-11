var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var MessageQueue = require('message-queue-pebble');

var console = {};
console.log = function(){};

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
  
  var dict = {};
  dict['T24HOUR'] = 1;
  
  // Send to watchapp
  MessageQueue.sendAppMessage(dict, function() {
    console.log('Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('Send failed!');
  });
  
});

var myAPIKey = 'aaa2844f92ab599ab4ea966a6da8a965';
var tfhour_set = 0;

function fetchWeather(latitude, longitude) {
  console.log('fetchWeather(): requesting weather');
  var req = new XMLHttpRequest();
  req.open('GET', 'http://api.openweathermap.org/data/2.5/weather?' +
    'lat=' + latitude + '&lon=' + longitude + '&cnt=1&appid=' + myAPIKey +'&_=' + new Date().getTime(), true);
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        console.log(req.responseText);
        var response = JSON.parse(req.responseText);
        var temperature_F = Math.round(1.8 * (response.main.temp - 273.15) + 32);
        var code = response.weather[0].id;
        
        var d = new Date();
        var timestamp = Math.round(d.getTime() / 1000);
        
        var sunrise = response.sys.sunrise;
        var sunset = response.sys.sunset;
        
        console.log(temperature_F);
        console.log(code);
        console.log('sunrise=' + sunrise + ' sunset=' + sunset);
        console.log(timestamp);
        MessageQueue.sendAppMessage({
          'WEATHER_ICON': code,
          'WEATHER_TEMPERATURE': temperature_F,
          'WEATHER_SUNRISE': sunrise,
          'WEATHER_SUNSET': sunset,
          'WEATHER_TIMESTAMP': timestamp
        });
      } else {
        console.log('fetchWeather(): error ' + req.status);
      }
    }
  };
  req.send(null);
}

function getIPlocation() {
  console.log('getIPlocation(): requesting location');
  var req = new XMLHttpRequest();
  req.open('GET', 'http://freegeoip.net/json/');
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        console.log(req.responseText);
        var response = JSON.parse(req.responseText);
        
        fetchWeather(response.latitude, response.longitude);
      }
    } else {
      console.log('getIPlocation(): error ' + req.status);
      
      MessageQueue.sendAppMessage({
        'WEATHER_ICON': 0,
        'WEATHER_TEMPERATURE': 0,
        'WEATHER_SUNRISE': 0,
        'WEATHER_SUNSET': 0,
        'WEATHER_TIMESTAMP': 0
      });
    }
  };
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.log('locationError(): error (' + err.code + '): ' + err.message);
  getIPlocation();
}

var locationOptions = {
  'timeout': 3000,
  'maximumAge': 180000
};

Pebble.addEventListener('appmessage', function (e) {
  console.log('received message!');

  if ( e.payload['T24HOUR'] ) {
    console.log("24 hour set");
    tfhour_set = 1;
  } else {
    window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
      locationOptions);
    console.log('received message to fetch weather!');
  }
});
