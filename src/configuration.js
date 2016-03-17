"use strict";(function(root,factory){if(typeof define==="function"&&define.amd){define([],factory)}else if(typeof module==="object"&&module.exports){module.exports=factory()}else{root.MessageQueue=factory()}})(this,function(){var RETRY_MAX=5;var queue=[];var sending=false;var timer=null;return{reset:reset,sendAppMessage:sendAppMessage,size:size};function reset(){queue=[];sending=false}function sendAppMessage(message,ack,nack){if(!isValidMessage(message)){return false}queue.push({message:message,ack:ack||null,nack:nack||null,attempts:0});setTimeout(function(){sendNextMessage()},1);return true}function size(){return queue.length}function isValidMessage(message){if(message!==Object(message)){return false}var keys=Object.keys(message);if(!keys.length){return false}for(var k=0;k<keys.length;k+=1){var validKey=/^[0-9a-zA-Z-_]*$/.test(keys[k]);if(!validKey){return false}var value=message[keys[k]];if(!validValue(value)){return false}}return true;function validValue(value){switch(typeof value){case"string":return true;case"number":return true;case"object":if(toString.call(value)==="[object Array]"){return true}}return false}}function sendNextMessage(){if(sending){return}var message=queue.shift();if(!message){return}message.attempts+=1;sending=true;Pebble.sendAppMessage(message.message,ack,nack);timer=setTimeout(function(){timeout()},1e3);function ack(){clearTimeout(timer);setTimeout(function(){sending=false;sendNextMessage()},200);if(message.ack){message.ack.apply(null,arguments)}}function nack(){clearTimeout(timer);if(message.attempts<RETRY_MAX){queue.unshift(message);setTimeout(function(){sending=false;sendNextMessage()},200*message.attempts)}else{if(message.nack){message.nack.apply(null,arguments)}}}function timeout(){setTimeout(function(){sending=false;sendNextMessage()},1e3);if(message.ack){message.ack.apply(null,arguments)}}}});

//var console = {};
//console.log = function(){};

(function () { "use strict";

// shortcuts for easier to read formulas

var PI   = Math.PI,
    sin  = Math.sin,
    cos  = Math.cos,
    tan  = Math.tan,
    asin = Math.asin,
    atan = Math.atan2,
    acos = Math.acos,
    rad  = PI / 180;

// sun calculations are based on http://aa.quae.nl/en/reken/zonpositie.html formulas


// date/time constants and conversions

var dayMs = 1000 * 60 * 60 * 24,
    J1970 = 2440588,
    J2000 = 2451545;

function toJulian(date) {
    return date.valueOf() / dayMs - 0.5 + J1970;
}
function fromJulian(j) {
    return new Date((j + 0.5 - J1970) * dayMs);
}
function toDays(date) {
    return toJulian(date) - J2000;
}


// general calculations for position

var e = rad * 23.4397; // obliquity of the Earth

function getRightAscension(l, b) {
    return atan(sin(l) * cos(e) - tan(b) * sin(e), cos(l));
}
function getDeclination(l, b) {
    return asin(sin(b) * cos(e) + cos(b) * sin(e) * sin(l));
}
function getAzimuth(H, phi, dec) {
    return atan(sin(H), cos(H) * sin(phi) - tan(dec) * cos(phi));
}
function getAltitude(H, phi, dec) {
    return asin(sin(phi) * sin(dec) + cos(phi) * cos(dec) * cos(H));
}
function getSiderealTime(d, lw) {
    return rad * (280.16 + 360.9856235 * d) - lw;
}


// general sun calculations

function getSolarMeanAnomaly(d) {
    return rad * (357.5291 + 0.98560028 * d);
}
function getEquationOfCenter(M) {
    return rad * (1.9148 * sin(M) + 0.02 * sin(2 * M) + 0.0003 * sin(3 * M));
}
function getEclipticLongitude(M, C) {
    var P = rad * 102.9372; // perihelion of the Earth
    return M + C + P + PI;
}
function getSunCoords(d) {

    var M = getSolarMeanAnomaly(d),
        C = getEquationOfCenter(M),
        L = getEclipticLongitude(M, C);

    return {
        dec: getDeclination(L, 0),
        ra: getRightAscension(L, 0)
    };
}


var SunCalc = {};


// calculates sun position for a given date and latitude/longitude

SunCalc.getPosition = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        c  = getSunCoords(d),
        H  = getSiderealTime(d, lw) - c.ra;

    return {
        azimuth: getAzimuth(H, phi, c.dec),
        altitude: getAltitude(H, phi, c.dec)
    };
};


// sun times configuration (angle, morning name, evening name)

var times = [
    [-0.83, 'sunrise',       'sunset'      ],
    [ -0.3, 'sunriseEnd',    'sunsetStart' ],
    [   -6, 'dawn',          'dusk'        ],
    [  -12, 'nauticalDawn',  'nauticalDusk'],
    [  -18, 'nightEnd',      'night'       ],
    [    6, 'goldenHourEnd', 'goldenHour'  ]
];

// adds a custom time to the times config

SunCalc.addTime = function (angle, riseName, setName) {
    times.push([angle, riseName, setName]);
};


// calculations for sun times

var J0 = 0.0009;

function getJulianCycle(d, lw) {
    return Math.round(d - J0 - lw / (2 * PI));
}
function getApproxTransit(Ht, lw, n) {
    return J0 + (Ht + lw) / (2 * PI) + n;
}
function getSolarTransitJ(ds, M, L) {
    return J2000 + ds + 0.0053 * sin(M) - 0.0069 * sin(2 * L);
}
function getHourAngle(h, phi, d) {
    return acos((sin(h) - sin(phi) * sin(d)) / (cos(phi) * cos(d)));
}


// calculates sun times for a given date and latitude/longitude

SunCalc.getTimes = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        n  = getJulianCycle(d, lw),
        ds = getApproxTransit(0, lw, n),

        M = getSolarMeanAnomaly(ds),
        C = getEquationOfCenter(M),
        L = getEclipticLongitude(M, C),

        dec = getDeclination(L, 0),

        Jnoon = getSolarTransitJ(ds, M, L);


    // returns set time for the given sun altitude
    function getSetJ(h) {
        var w = getHourAngle(h, phi, dec),
            a = getApproxTransit(w, lw, n);

        return getSolarTransitJ(a, M, L);
    }


    var result = {
        solarNoon: fromJulian(Jnoon),
        nadir: fromJulian(Jnoon - 0.5)
    };

    var i, len, time, angle, morningName, eveningName, Jset, Jrise;

    for (i = 0, len = times.length; i < len; i += 1) {
        time = times[i];

        Jset = getSetJ(time[0] * rad);
        Jrise = Jnoon - (Jset - Jnoon);

        result[time[1]] = fromJulian(Jrise);
        result[time[2]] = fromJulian(Jset);
    }

    return result;
};


// moon calculations, based on http://aa.quae.nl/en/reken/hemelpositie.html formulas

function getMoonCoords(d) { // geocentric ecliptic coordinates of the moon

    var L = rad * (218.316 + 13.176396 * d), // ecliptic longitude
        M = rad * (134.963 + 13.064993 * d), // mean anomaly
        F = rad * (93.272 + 13.229350 * d),  // mean distance

        l  = L + rad * 6.289 * sin(M), // longitude
        b  = rad * 5.128 * sin(F),     // latitude
        dt = 385001 - 20905 * cos(M);  // distance to the moon in km

    return {
        ra: getRightAscension(l, b),
        dec: getDeclination(l, b),
        dist: dt
    };
}

SunCalc.getMoonPosition = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        c = getMoonCoords(d),
        H = getSiderealTime(d, lw) - c.ra,
        h = getAltitude(H, phi, c.dec);

    // altitude correction for refraction
    h = h + rad * 0.017 / tan(h + rad * 10.26 / (h + rad * 5.10));

    return {
        azimuth: getAzimuth(H, phi, c.dec),
        altitude: h,
        distance: c.dist
    };
};


// calculations for illuminated fraction of the moon,
// based on http://idlastro.gsfc.nasa.gov/ftp/pro/astro/mphase.pro formulas

SunCalc.getMoonFraction = function (date) {

    var d = toDays(date),
        s = getSunCoords(d),
        m = getMoonCoords(d),

        sdist = 149598000, // distance from Earth to Sun in km

        phi = acos(sin(s.dec) * sin(m.dec) + cos(s.dec) * cos(m.dec) * cos(s.ra - m.ra)),
        inc = atan(sdist * sin(phi), m.dist - sdist * cos(phi));

    return (1 + cos(inc)) / 2;
};


// export as AMD module / Node module / browser variable

if (typeof define === 'function' && define.amd) {
    define(SunCalc);
} else if (typeof module !== 'undefined') {
    module.exports = SunCalc;
} else {
    window.SunCalc = SunCalc;
}

}());

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
        var sunTimes = SunCalc.getTimes(d, latitude, longitude);
        var sunrise = Math.round(sunTimes.sunrise.getTime() / 1000);
        var sunset = Math.round(sunTimes.sunset.getTime() / 1000);
        var timestamp = Math.round(d.getTime() / 1000);
        
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
