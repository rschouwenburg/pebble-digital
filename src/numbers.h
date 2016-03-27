#pragma once

#include "pebble.h"

#define DARK_VERSION 1
//#define SCREENSHOT 1

struct weather_icons {
  uint16_t code;
  uint8_t x;
  uint8_t y;
};

typedef struct weather_icons WeatherIcons;

static const uint16_t WEATHER_ICON_SPRITE_LINES[] = { 
  0, 23, 48, 73, 96, 121, 141, 163, 186, 209, 231, 252, 274, 297, 318
};
  
static const WeatherIcons WEATHER_DAY_ICONS_CODE_DEF[] = {
  { 0, 1, 10 }, // no report or unknown code
  { 200, 1, 6 }, //	thunderstorm with light rain	 11d
  { 201, 1, 6	}, // thunderstorm with rain	 11d
  { 202, 1, 6	}, // thunderstorm with heavy rain	 11d
  { 210, 2, 4	}, // light thunderstorm	 11d
  { 211, 2, 6	}, // thunderstorm	 11d
  { 212, 2, 6	}, // heavy thunderstorm	 11d
  { 221, 2, 6	}, // ragged thunderstorm	 11d
  { 230, 1, 6	}, // thunderstorm with light drizzle	 11d
  { 231, 1, 6	}, // thunderstorm with drizzle	 11d
  { 232, 1, 6	}, // thunderstorm with heavy drizzle	 11d
    
  { 300, 0, 6	}, // light intensity drizzle	 09d
  { 301, 0, 6 }, // drizzle	 09d
  { 302, 0, 6	}, // heavy intensity drizzle	 09d
  { 310, 0, 6	}, // light intensity drizzle rain	 09d
  { 311, 0, 6	}, // drizzle rain	 09d
  { 312, 0, 6	}, // heavy intensity drizzle rain	 09d
  { 313, 2, 0	}, // shower rain and drizzle	 09d
  { 314, 1, 1	}, // heavy shower rain and drizzle	 09d
  { 321, 2, 0	}, // shower drizzle	 09d
    
  { 500, 0, 6	}, // light rain	 10d
  { 501, 2, 0	}, // moderate rain	 10d
  { 502, 1, 1	}, // heavy intensity rain	 10d
  { 503, 1, 1	}, // very heavy rain	 10d
  { 504, 1, 1	}, // extreme rain	 10d
  { 511, 2, 8	}, // freezing rain	 13d
  { 520, 0, 6	}, // light intensity shower rain	 09d
  { 521, 2, 0	}, // shower rain	 09d
  { 522, 1, 1	}, // heavy int ensity shower rain	 09d
  { 531, 2, 0	}, // ragged shower rain
    
  { 600, 0, 8	}, // light snow	[[file:13d.png]]
  { 601, 0, 8	}, // snow	[[file:13d.png]]
  { 602, 0, 1	}, // heavy snow	[[file:13d.png]]
  { 611, 2, 8	}, // sleet	[[file:13d.png]]
  { 612, 2, 8	}, // shower	[[file:13d.png]]
  { 615, 2, 8	}, // light rain and snow	[[file:13d.png]]
  { 616, 2, 8	}, // rain and snow	[[file:13d.png]]
  { 620, 0, 8	}, // light shower snow	[[file:13d.png]]
  { 621, 0, 8	}, // shower snow	[[file:13d.png]]
  { 622, 0, 1	}, // heavy shower snow	[[file:13d.png]]
  
  { 701, 0, 10 }, // mist	[[file:50d.png]]
  { 711, 2, 10	}, // smoke	[[file:50d.png]]
  { 721, 0, 13 }, // haze	[[file:50d.png]]
  { 731, 2, 10	}, // sand, dust whirls	[[file:50d.png]]
  { 741, 0, 10 }, // fog	[[file:50d.png]]
  { 751, 2, 10	}, // sand	[[file:50d.png]]
  { 761, 2, 10	}, // dust	[[file:50d.png]]
  { 762, 2, 10	}, // volcanic ash	[[file:50d.png]]
  { 771, 2, 10	}, // squalls	[[file:50d.png]]
  { 781, 2, 12	}, // tornado	[[file:50d.png]]
    
  { 800, 1, 9	}, // clear sky	[[file:01d.png]] [[file:01n.png]]
  
  { 801, 1, 9 }, //	few clouds	[[file:02d.png]] [[file:02n.png]]
  { 802, 1, 11	}, // scattered clouds	[[file:03d.png]] [[file:03d.png]]
  { 803, 1, 11	}, // broken clouds	[[file:04d.png]] [[file:03d.png]]
  { 804, 1, 8	}, // overcast clouds	[[file:04d.png]] [[file:04d.png]]
    
  { 900, 2, 12 }, // tornado
  { 901, 2, 13 }, // tropical storm
  { 902, 1, 3 }, // hurricane
  { 903, 1, 4	}, // cold
  { 904, 1, 2	}, // hot
  { 905, 0, 3	}, // windy
  { 906, 0, 12 }, // hail
    
  { 951, 1, 9	}, // calm
  { 952, 1, 9	}, // light breeze
  { 953, 1, 9	}, // gentle breeze
  { 954, 0, 3	}, // moderate breeze
  { 955, 0, 3	}, // fresh breeze
  { 956, 0, 3	}, // strong breeze
  { 957, 0, 3	}, // high wind, near gale
  { 958, 0, 3	}, // gale
  { 959, 0, 3	}, // severe gale
  { 960, 0, 3	}, // storm
  { 961, 0, 3	}, // violent storm
  { 962, 1, 3 }, // hurricane
};

static const WeatherIcons WEATHER_NIGHT_ICONS_CODE_DEF[] = {
  { 0, 1, 10 }, // no report or unknown code
  { 200, 1, 5 }, //	thunderstorm with light rain	 11d
  { 201, 1, 5	}, // thunderstorm with rain	 11d
  { 202, 1, 5	}, // thunderstorm with heavy rain	 11d
  { 210, 2, 3	}, // light thunderstorm	 11d
  { 211, 2, 5	}, // thunderstorm	 11d
  { 212, 2, 5	}, // heavy thunderstorm	 11d
  { 221, 2, 5	}, // ragged thunderstorm	 11d
  { 230, 1, 5	}, // thunderstorm with light drizzle	 11d
  { 231, 1, 5	}, // thunderstorm with drizzle	 11d
  { 232, 1, 5	}, // thunderstorm with heavy drizzle	 11d
    
  { 300, 0, 5	}, // light intensity drizzle	 09d
  { 301, 0, 5 }, // drizzle	 09d
  { 302, 0, 5	}, // heavy intensity drizzle	 09d
  { 310, 0, 5	}, // light intensity drizzle rain	 09d
  { 311, 0, 5	}, // drizzle rain	 09d
  { 312, 0, 5	}, // heavy intensity drizzle rain	 09d
  { 313, 1, 13 }, // shower rain and drizzle	 09d
  { 314, 1, 0	}, // heavy shower rain and drizzle	 09d
  { 321, 1, 13	}, // shower drizzle	 09d
    
  { 500, 0, 5	}, // light rain	 10d
  { 501, 1, 13	}, // moderate rain	 10d
  { 502, 1, 0	}, // heavy intensity rain	 10d
  { 503, 1, 0	}, // very heavy rain	 10d
  { 504, 1, 0	}, // extreme rain	 10d
  { 511, 2, 7	}, // freezing rain	 13d
  { 520, 0, 5	}, // light intensity shower rain	 09d
  { 521, 1, 13	}, // shower rain	 09d
  { 522, 1, 0	}, // heavy int ensity shower rain	 09d
  { 531, 1, 13	}, // ragged shower rain
    
  { 600, 0, 8	}, // light snow	[[file:13d.png]]
  { 601, 0, 8	}, // snow	[[file:13d.png]]
  { 602, 0, 0	}, // heavy snow	[[file:13d.png]]
  { 611, 2, 7	}, // sleet	[[file:13d.png]]
  { 612, 2, 7	}, // shower ScrollLayeret	[[file:13d.png]]
  { 615, 2, 7	}, // light rain and snow	[[file:13d.png]]
  { 616, 2, 7	}, // rain and snow	[[file:13d.png]]
  { 620, 0, 8	}, // light shower snow	[[file:13d.png]]
  { 621, 0, 8	}, // shower snow	[[file:13d.png]]
  { 622, 0, 0	}, // heavy shower snow	[[file:13d.png]]
  
  { 701, 0, 9 }, // mist	[[file:50d.png]]
  { 711, 2, 10	}, // smoke	[[file:50d.png]]
  { 721, 0, 9 }, // haze	[[file:50d.png]]
  { 731, 2, 10	}, // sand, dust whirls	[[file:50d.png]]
  { 741, 0, 9 }, // fog	[[file:50d.png]]
  { 751, 2, 10	}, // sand	[[file:50d.png]]
  { 761, 2, 10	}, // dust	[[file:50d.png]]
  { 762, 2, 10	}, // volcanic ash	[[file:50d.png]]
  { 771, 2, 10	}, // squalls	[[file:50d.png]]
  { 781, 2, 12	}, // tornado	[[file:50d.png]]
    
  { 800, 0, 4	}, // clear sky	[[file:01d.png]] [[file:01n.png]]
  
  { 801, 0, 4 }, //	few clouds	[[file:02d.png]] [[file:02n.png]]
  { 802, 1, 12 }, // scattered clouds	[[file:03d.png]] [[file:03d.png]]
  { 803, 1, 12 }, // broken clouds	[[file:04d.png]] [[file:03d.png]]
  { 804, 1, 7	}, // overcast clouds	[[file:04d.png]] [[file:04d.png]]
    
  { 900, 2, 12 }, // tornado
  { 901, 2, 13 }, // tropical storm
  { 902, 1, 3 }, // hurricane
  { 903, 1, 4	}, // cold
  { 904, 1, 2	}, // hot
  { 905, 0, 3	}, // windy
  { 906, 0, 11 }, // hail
    
  { 951, 0, 4	}, // calm
  { 952, 0, 4	}, // light breeze
  { 953, 0, 4	}, // gentle breeze
  { 954, 0, 3	}, // moderate breeze
  { 955, 0, 3	}, // fresh breeze
  { 956, 0, 3	}, // strong breeze
  { 957, 0, 3	}, // high wind, near gale
  { 958, 0, 3	}, // gale
  { 959, 0, 3	}, // severe gale
  { 960, 0, 3	}, // storm
  { 961, 0, 3	}, // violent storm
  { 962, 1, 3 }, // hurricane
};

static const GRect NUMBER_SPRITE_DEF[] = {
  { {0, 0}, {24, 42} },    // 1
  { {28, 0}, {32, 42} },   // 2
  { {60, 0}, {32, 42} },   // 3
  { {91, 0}, {34, 42} },  // 4
  { {125, 0}, {31, 42} },  // 5
  { {156, 0}, {31, 42} },  // 6
  { {186, 0}, {32, 42} },  // 7
  { {218, 0}, {32, 42} },  // 8
  { {249, 0}, {34, 42} },  // 9
  { {283, 0}, {32, 42} },  // 0
  { {316, 9}, {11, 42} },   // :
};

static const GRect ALPANUMERIC_SPRITE_DEF[] = {
  { {0, 0}, {12, 16} },    // A
  { {15, 0}, {9, 16} },   // B
  { {28, 0}, {10, 16} },   // C
  { {42, 0}, {10, 16} },  // D
  { {56, 0}, {9, 16} },  // E
  { {69, 0}, {9, 16} },  // F
  { {81, 0}, {11, 16} },  // G
  { {95, 0}, {12, 16} },  // H
  { {110, 0}, {6, 16} },  // I
  { {118, 0}, {8, 16} },  // J
  { {131, 0}, {11, 16} },   // K
  { {145, 0}, {8, 16} },   // L
  { {157, 0}, {13, 16} },   // M
  { {175, 0}, {10, 16} },   // N
  { {190, 0}, {11, 16} },   // O
  { {205, 0}, {10, 16} },   // P
  { {218, 0}, {11, 16} },   // Q
  { {233, 0}, {10, 16} },   // R
  { {246, 0}, {10, 16} },   // S
  { {259, 0}, {11, 16} },   // T
  { {273, 0}, {10, 16} },   // U
  { {287, 0}, {11, 16} },   // V
  { {300, 0}, {16, 16} },   // W
  { {319, 0}, {10, 16} },   // X
  { {332, 0}, {11, 16} },   // Y
  { {346, 0}, {9, 16} },   // Z
  { {359, 0}, {8, 16} },   // 0
  { {370, 0}, {8, 16} },   // 1
  { {384, 0}, {9, 16} },   // 2
  { {396, 0}, {9, 16} },   // 3
  { {408, 0}, {10, 16} },   // 4
  { {421, 0}, {8, 16} },   // 5
  { {433, 0}, {9, 16} },   // 6
  { {445, 0}, {10, 16} },   // 7
  { {458, 0}, {8, 16} },   // 8
  { {470, 0}, {9, 16} },   // 9
  { {482, 0}, {6, 16} },    // -
  { {490, 0}, {8, 16} }     // degree symbol
};

static const GRect WEATHER_ICON_SPRITE_DEF[] = {
  { {0, 0}, {25, 27} },    // 0 Sunny
  { {42, 0}, {25, 27} },   // 1 Cloudy
  { {84, 0}, {25, 27} },   // 2 Rainy
  { {127, 0}, {22, 27} }   // 3 Snowy
};


static const struct GPathInfo BT_ICON_PATH = {
  6,
    (GPoint []) {
      {1, 5},
      {8, 12},
      {4, 16},
      {4, 1},
      {8, 5},
      {1, 12}
    }
};

static const struct GPathInfo CHARGING_ICON_PATH = {
  7,
    (GPoint []) {
      {5, 1},
      {1, 5},
      {3, 5},
      {2, 9},
      {6, 4},
      {3, 4},
      {5, 1}
    }
};
