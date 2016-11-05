#pragma once

#include "pebble.h"

#define DARK_VERSION 1
//#define SCREENSHOT 1

struct weather_icons {
  uint16_t code;
  uint8_t x;
};

typedef struct weather_icons WeatherIcons;
  
static const WeatherIcons WEATHER_DAY_ICONS_CODE_DEF[] = {
  { 0, 21 }, // no report or unknown code
  { 200, 22 }, //	thunderstorm with light rain	 11d
  { 201, 22	}, // thunderstorm with rain	 11d
  { 202, 22	}, // thunderstorm with heavy rain	 11d
  { 210, 23	}, // light thunderstorm	 11d
  { 211, 23	}, // thunderstorm	 11d
  { 212, 23	}, // heavy thunderstorm	 11d
  { 221, 23	}, // ragged thunderstorm	 11d
  { 230, 19	}, // thunderstorm with light drizzle	 11d
  { 231, 19	}, // thunderstorm with drizzle	 11d
  { 232, 19	}, // thunderstorm with heavy drizzle	 11d
    
  { 300, 18	}, // light intensity drizzle	 09d
  { 301, 18 }, // drizzle	 09d
  { 302, 18	}, // heavy intensity drizzle	 09d
  { 310, 18	}, // light intensity drizzle rain	 09d
  { 311, 18	}, // drizzle rain	 09d
  { 312, 18	}, // heavy intensity drizzle rain	 09d
  { 313, 18	}, // shower rain and drizzle	 09d
  { 314, 18	}, // heavy shower rain and drizzle	 09d
  { 321, 18	}, // shower drizzle	 09d
    
  { 500, 20	}, // light rain	 10d
  { 501, 20	}, // moderate rain	 10d
  { 502, 20	}, // heavy intensity rain	 10d
  { 503, 20	}, // very heavy rain	 10d
  { 504, 20	}, // extreme rain	 10d
  { 511, 17	}, // freezing rain	 13d
  { 520, 16	}, // light intensity shower rain	 09d
  { 521, 16	}, // shower rain	 09d
  { 522, 16	}, // heavy int ensity shower rain	 09d
  { 531, 16	}, // ragged shower rain
    
  { 600, 14	}, // light snow	[[file:13d.png]]
  { 601, 14	}, // snow	[[file:13d.png]]
  { 602, 14	}, // heavy snow	[[file:13d.png]]
  { 611, 17	}, // sleet	[[file:13d.png]]
  { 612, 17	}, // shower sleet	[[file:13d.png]]
  { 615, 14	}, // light rain and snow	[[file:13d.png]]
  { 616, 14	}, // rain and snow	[[file:13d.png]]
  { 620, 14	}, // light shower snow	[[file:13d.png]]
  { 621, 14	}, // shower snow	[[file:13d.png]]
  { 622, 14	}, // heavy shower snow	[[file:13d.png]]
  
  { 701, 15 }, // mist	[[file:50d.png]]
  { 711, 12	}, // smoke	[[file:50d.png]]
  { 721, 13 }, // haze	[[file:50d.png]]
  { 731, 11	}, // sand, dust whirls	[[file:50d.png]]
  { 741, 15 }, // fog	[[file:50d.png]]
  { 751, 11	}, // sand	[[file:50d.png]]
  { 761, 11	}, // dust	[[file:50d.png]]
  { 762, 11	}, // volcanic ash	[[file:50d.png]]
  { 771, 10 }, // squalls	[[file:50d.png]]
  { 781, 6}, // tornado	[[file:50d.png]]
    
  { 800, 5 }, // clear sky	[[file:01d.png]] [[file:01n.png]]
  
  { 801, 5 }, //	few clouds	[[file:02d.png]] [[file:02n.png]]
  { 802, 3	}, // scattered clouds	[[file:03d.png]] [[file:03d.png]]
  { 803, 3	}, // broken clouds	[[file:04d.png]] [[file:03d.png]]
  { 804, 4	}, // overcast clouds	[[file:04d.png]] [[file:04d.png]]
    
  { 900, 6 }, // tornado
  { 901, 2 }, // tropical storm
  { 902, 2 }, // hurricane
  { 903, 9	}, // cold
  { 904, 8	}, // hot
  { 905, 10	}, // windy
  { 906, 7 }, // hail
    
  { 951, 5	}, // calm
  { 952, 5	}, // light breeze
  { 953, 5	}, // gentle breeze
  { 954, 5	}, // moderate breeze
  { 955, 5	}, // fresh breeze
  { 956, 10	}, // strong breeze
  { 957, 10	}, // high wind, near gale
  { 958, 10	}, // gale
  { 959, 10	}, // severe gale
  { 960, 10	}, // storm
  { 961, 10	}, // violent storm
  { 962, 2 }, // hurricane
};

static const WeatherIcons WEATHER_NIGHT_ICONS_CODE_DEF[] = {
  { 0, 21 }, // no report or unknown code
  { 200, 31 }, //	thunderstorm with light rain	 11d
  { 201, 31	}, // thunderstorm with rain	 11d
  { 202, 31	}, // thunderstorm with heavy rain	 11d
  { 210, 26	}, // light thunderstorm	 11d
  { 211, 26	}, // thunderstorm	 11d
  { 212, 26	}, // heavy thunderstorm	 11d
  { 221, 26	}, // ragged thunderstorm	 11d
  { 230, 32	}, // thunderstorm with light drizzle	 11d
  { 231, 32	}, // thunderstorm with drizzle	 11d
  { 232, 32	}, // thunderstorm with heavy drizzle	 11d
    
  { 300, 30	}, // light intensity drizzle	 09d
  { 301, 30 }, // drizzle	 09d
  { 302, 30	}, // heavy intensity drizzle	 09d
  { 310, 30	}, // light intensity drizzle rain	 09d
  { 311, 30	}, // drizzle rain	 09d
  { 312, 30	}, // heavy intensity drizzle rain	 09d
  { 313, 30	}, // shower rain and drizzle	 09d
  { 314, 30	}, // heavy shower rain and drizzle	 09d
  { 321, 30	}, // shower drizzle	 09d
    
  { 500, 24	}, // light rain	 10d
  { 501, 24	}, // moderate rain	 10d
  { 502, 24	}, // heavy intensity rain	 10d
  { 503, 24	}, // very heavy rain	 10d
  { 504, 24	}, // extreme rain	 10d
  { 511, 25	}, // freezing rain	 13d
  { 520, 28	}, // light intensity shower rain	 09d
  { 521, 28	}, // shower rain	 09d
  { 522, 28	}, // heavy int ensity shower rain	 09d
  { 531, 28	}, // ragged shower rain
    
  { 600, 14	}, // light snow	[[file:13d.png]]
  { 601, 14	}, // snow	[[file:13d.png]]
  { 602, 14	}, // heavy snow	[[file:13d.png]]
  { 611, 25	}, // sleet	[[file:13d.png]]
  { 612, 25	}, // shower sleet	[[file:13d.png]]
  { 615, 14	}, // light rain and snow	[[file:13d.png]]
  { 616, 14	}, // rain and snow	[[file:13d.png]]
  { 620, 14	}, // light shower snow	[[file:13d.png]]
  { 621, 14	}, // shower snow	[[file:13d.png]]
  { 622, 14	}, // heavy shower snow	[[file:13d.png]]
  
  { 701, 15 }, // mist	[[file:50d.png]]
  { 711, 12	}, // smoke	[[file:50d.png]]
  { 721, 13 }, // haze	[[file:50d.png]]
  { 731, 11	}, // sand, dust whirls	[[file:50d.png]]
  { 741, 15 }, // fog	[[file:50d.png]]
  { 751, 11	}, // sand	[[file:50d.png]]
  { 761, 11	}, // dust	[[file:50d.png]]
  { 762, 11	}, // volcanic ash	[[file:50d.png]]
  { 771, 10 }, // squalls	[[file:50d.png]]
  { 781, 6}, // tornado	[[file:50d.png]]
    
  { 800, 33 }, // clear sky	[[file:01d.png]] [[file:01n.png]]
  
  { 801, 33 }, //	few clouds	[[file:02d.png]] [[file:02n.png]]
  { 802, 29	}, // scattered clouds	[[file:03d.png]] [[file:03d.png]]
  { 803, 29	}, // broken clouds	[[file:04d.png]] [[file:03d.png]]
  { 804, 27	}, // overcast clouds	[[file:04d.png]] [[file:04d.png]]
    
  { 900, 6 }, // tornado
  { 901, 2 }, // tropical storm
  { 902, 2 }, // hurricane
  { 903, 9	}, // cold
  { 904, 8	}, // hot
  { 905, 37	}, // windy
  { 906, 34 }, // hail
    
  { 951, 33	}, // calm
  { 952, 33	}, // light breeze
  { 953, 33	}, // gentle breeze
  { 954, 33	}, // moderate breeze
  { 955, 33	}, // fresh breeze
  { 956, 37	}, // strong breeze
  { 957, 37	}, // high wind, near gale
  { 958, 37	}, // gale
  { 959, 37	}, // severe gale
  { 960, 37	}, // storm
  { 961, 37	}, // violent storm
  { 962, 2 }, // hurricane
};

static const GRect NUMBER_SPRITE_DEF[] = {
  { {0, 0}, {24, 43} },    // 1
  { {30, 0}, {32, 43} },   // 2
  { {62, 0}, {31, 43} },   // 3
  { {93, 0}, {35, 43} },  // 4
  { {128, 0}, {31, 43} },  // 5
  { {159, 0}, {30, 43} },  // 6
  { {189, 0}, {34, 43} },  // 7
  { {223, 0}, {30, 43} },  // 8
  { {254, 0}, {33, 43} },  // 9
  { {287, 0}, {35, 43} },  // 0
  { {322, 9}, {11, 43} },   // :
};

static const GRect ALPANUMERIC_SPRITE_DEF[] = {
  { {0, 0}, {12, 17} },    // A
  { {15, 0}, {9, 17} },   // B
  { {28, 0}, {10, 17} },   // C
  { {42, 0}, {10, 17} },  // D
  { {56, 0}, {9, 17} },  // E
  { {69, 0}, {9, 17} },  // F
  { {81, 0}, {11, 17} },  // G
  { {95, 0}, {12, 17} },  // H
  { {110, 0}, {6, 17} },  // I
  { {118, 0}, {8, 17} },  // J
  { {131, 0}, {11, 17} },   // K
  { {145, 0}, {8, 17} },   // L
  { {157, 0}, {13, 17} },   // M
  { {175, 0}, {10, 17} },   // N
  { {190, 0}, {11, 17} },   // O
  { {205, 0}, {10, 17} },   // P
  { {218, 0}, {11, 17} },   // Q
  { {233, 0}, {10, 17} },   // R
  { {246, 0}, {10, 17} },   // S
  { {259, 0}, {11, 17} },   // T
  { {273, 0}, {10, 17} },   // U
  { {287, 0}, {11, 17} },   // V
  { {300, 0}, {16, 17} },   // W
  { {319, 0}, {10, 17} },   // X
  { {332, 0}, {11, 17} },   // Y
  { {346, 0}, {9, 17} },   // Z
  { {359, 0}, {8, 17} },   // 0
  { {370, 0}, {8, 17} },   // 1
  { {384, 0}, {9, 17} },   // 2
  { {396, 0}, {9, 17} },   // 3
  { {408, 0}, {10, 17} },   // 4
  { {421, 0}, {8, 17} },   // 5
  { {433, 0}, {9, 17} },   // 6
  { {445, 0}, {10, 17} },   // 7
  { {458, 0}, {8, 17} },   // 8
  { {470, 0}, {9, 17} },   // 9
  { {482, 0}, {6, 17} },    // -
  { {490, 0}, {8, 17} }     // degree symbol
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
