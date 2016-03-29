"use strict";(function(root,factory){if(typeof define==="function"&&define.amd){define([],factory)}else if(typeof module==="object"&&module.exports){module.exports=factory()}else{root.MessageQueue=factory()}})(this,function(){var RETRY_MAX=5;var queue=[];var sending=false;var timer=null;return{reset:reset,sendAppMessage:sendAppMessage,size:size};function reset(){queue=[];sending=false}function sendAppMessage(message,ack,nack){if(!isValidMessage(message)){return false}queue.push({message:message,ack:ack||null,nack:nack||null,attempts:0});setTimeout(function(){sendNextMessage()},1);return true}function size(){return queue.length}function isValidMessage(message){if(message!==Object(message)){return false}var keys=Object.keys(message);if(!keys.length){return false}for(var k=0;k<keys.length;k+=1){var validKey=/^[0-9a-zA-Z-_]*$/.test(keys[k]);if(!validKey){return false}var value=message[keys[k]];if(!validValue(value)){return false}}return true;function validValue(value){switch(typeof value){case"string":return true;case"number":return true;case"object":if(toString.call(value)==="[object Array]"){return true}}return false}}function sendNextMessage(){if(sending){return}var message=queue.shift();if(!message){return}message.attempts+=1;sending=true;Pebble.sendAppMessage(message.message,ack,nack);timer=setTimeout(function(){timeout()},1e3);function ack(){clearTimeout(timer);setTimeout(function(){sending=false;sendNextMessage()},200);if(message.ack){message.ack.apply(null,arguments)}}function nack(){clearTimeout(timer);if(message.attempts<RETRY_MAX){queue.unshift(message);setTimeout(function(){sending=false;sendNextMessage()},200*message.attempts)}else{if(message.nack){message.nack.apply(null,arguments)}}}function timeout(){setTimeout(function(){sending=false;sendNextMessage()},1e3);if(message.ack){message.ack.apply(null,arguments)}}}});

//var console = {};
//console.log = function(){};

Pebble.addEventListener('ready', function (e) {
  console.log('connect!' + e.ready);
  //window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
  //  locationOptions);
  console.log(e.type);
  
  var dict = {};
  dict['KEY_24HOUR'] = 1;
  
  // Send to watchapp
  MessageQueue.sendAppMessage(dict, function() {
    console.log('Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('Send failed!');
  });
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://pebble.schouwenburg.com/pebble/numbers-1-0.html';
  if ( tfhour_set == 1 ) {
    url = url + "?tfhour=1";
  }
  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  tfhour_set = 0;
  
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  var vibrate = configData['vibrate'] ? 1 : 0;
  var eudate = configData['eudate'];
  var bottombar = configData['bottombar'] ? 1 : 0;
  var topbar = configData['topbar'] ? 1 : 0;
  var celsius = configData['celsius'];
  
  var dict = {};
  dict['KEY_VIBRATE'] = parseInt(vibrate);
  dict['KEY_EUDATE'] = parseInt(eudate);
  dict['KEY_TOPBAR'] = parseInt(topbar);
  dict['KEY_BOTTOMBAR'] = parseInt(bottombar);
  dict['KEY_CELSIUS'] = parseInt(celsius);
  
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
          'WEATHER_ICON_KEY': code,
          'WEATHER_TEMPERATURE_KEY': temperature_F,
          'WEATHER_SUNRISE_KEY': sunrise,
          'WEATHER_SUNSET_KEY': sunset,
          'WEATHER_TIMESTAMP_KEY': timestamp
        });
      } else {
        console.log('fetchWeather(): error ' + req.status);
      }
    }
  };
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('locationError(): error (' + err.code + '): ' + err.message);
  MessageQueue.sendAppMessage({
    'WEATHER_ICON_KEY': 0,
    'WEATHER_TEMPERATURE_KEY': 0,
    'WEATHER_SUNRISE_KEY': 0,
    'WEATHER_SUNSET_KEY': 0,
    'WEATHER_TIMESTAMP_KEY': 0
  });
}

var locationOptions = {
  'timeout': 30000,
  'maximumAge': 180000
};

Pebble.addEventListener('appmessage', function (e) {
  if ( e.payload['KEY_24HOUR'] ) {
    console.log("24 hour set");
    tfhour_set = 1;
  } else {
    window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
      locationOptions);
    console.log(e.type);
    console.log(e.payload.temperature);
    console.log('message!');
  }
});
