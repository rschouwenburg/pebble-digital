#include "numbers.h"

#include "pebble.h"
#include <ctype.h>

#undef APP_LOG
#define APP_LOG(...)

#define KEY_VIBRATE    0
#define KEY_EUDATE       10
//#define WEATHER_ICON_KEY         2
//#define WEATHER_TEMPERATURE_KEY  3
//#define WEATHER_SUNRISE_KEY      4
//#define WEATHER_SUNSET_KEY      5
//#define WEATHER_TIMESTAMP_KEY  6
#define KEY_TOPBAR       7
#define KEY_BOTTOMBAR    8
#define KEY_CELSIUS      9
#define KEY_24HOUR      11

// 1800, 7200, 300
#define REQUEST_WEATHER_INTERVAL_SECS 900
#define WEATHER_VALID_FOR_SECS 7200
#define WEATHER_RETRY_INTERVAL_SECS 300

#ifdef PBL_ROUND
  #define UI_TOP_BAR_Y 16
  #define UI_DATE_BAR_Y 47
  #define UI_TIME_BAR_Y 78
  #define UI_BOTTOM_BAR_Y 122
#else
  #define UI_TOP_BAR_Y 10
  #define UI_DATE_BAR_Y 41
  #define UI_TIME_BAR_Y 73
  #define UI_BOTTOM_BAR_Y 119
#endif

static Window *s_window;

static Layer *s_simple_bg_layer, *s_time_layer, *s_date_layer, *s_bt_icon_layer, *s_bat_icon_layer, *s_weather_layer;
static TextLayer *s_temperature_layer;
static BitmapLayer *s_number_layer[5], *s_label_layer[10], *s_weather_icon_bmlayer, *s_temp_bmlayer[4];

static GPath *s_bt_icon_gpath, *s_charging_icon_gpath;

static GBitmap *s_numbers_sprite_bitmap, *s_number_bitmap[5], *s_alphanumeric_sprite_bitmap, *s_label_bitmap[10], 
               *s_weather_icons_bitmap, *s_weather_icon_bitmap, *s_temp_bitmap[4];

static char s_time_buffer[12], s_date_buffer[32];

bool first_run = 1;
int vibrate = 0;
int eudate = 0;
int topbar = 1;
int bottombar = 1;
int celsius = 0;

int counter = 0;

int notify_24hours_trigger = 0;

int retries = 0;

int bat_charge;

static AppSync s_sync;
static uint8_t s_sync_buffer[128];

enum WeatherKey {
  WEATHER_ICON_KEY = 0x2,         // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x3,  // TUPLE_CSTRING
  WEATHER_SUNRISE_KEY = 0x4,      // TUPLE_INT
  WEATHER_SUNSET_KEY = 0x5,       // TUPLE_INT
  WEATHER_TIMESTAMP_KEY = 0x6     // TUPLE_INT
};

struct weather {
  uint32_t timestamp;
  uint16_t temperature;
  uint16_t code;
  uint32_t sunrise;
  uint32_t sunset;
  uint32_t last_try;
};

typedef struct weather WeatherInfo;

static WeatherInfo current_weather = { 0, 999, 0, 0, 0, 0 };

static void battery_handler(BatteryChargeState charge_state) {
   layer_mark_dirty(s_bat_icon_layer);
}

static void notify_24hours(void) {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "notify 24 hour setting");
    
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
    // Error creating outbound message
    return;
  }

  int value = 1;
  dict_write_int(iter, KEY_24HOUR, &value, sizeof(int), true);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

  switch (key) {
    case WEATHER_TIMESTAMP_KEY:
      if ( new_tuple->value->uint32 > 0 ) {
        current_weather.timestamp = new_tuple->value->uint32;
        persist_write_int(WEATHER_TIMESTAMP_KEY, current_weather.timestamp);

        APP_LOG(APP_LOG_LEVEL_DEBUG, "received timestamp: %u", (int) current_weather.timestamp);
      }
    
      break;
    
    case WEATHER_ICON_KEY:
      if ( new_tuple->value->uint16 > 0 ) {
        current_weather.code = new_tuple->value->uint16;        
        persist_write_int(WEATHER_ICON_KEY, current_weather.code);
        //layer_mark_dirty(s_weather_layer);
        
        APP_LOG(APP_LOG_LEVEL_DEBUG, "received weather icon: %d", new_tuple->value->uint16);
      }

      break;

    case WEATHER_TEMPERATURE_KEY:
      if ( new_tuple->value->uint16 ) {
        current_weather.temperature = new_tuple->value->uint16;                
        persist_write_int(WEATHER_TEMPERATURE_KEY, current_weather.temperature);
        //layer_mark_dirty(s_weather_layer);
        
        APP_LOG(APP_LOG_LEVEL_DEBUG, "received weather temp: %d", current_weather.temperature);
      }

      break;

    case WEATHER_SUNRISE_KEY:
      if ( new_tuple->value->uint32 > 0 ) {
        current_weather.sunrise = new_tuple->value->uint32;
        persist_write_int(WEATHER_SUNRISE_KEY, current_weather.sunrise);
        
        APP_LOG(APP_LOG_LEVEL_DEBUG, "received sunrise at: %u",  (int) new_tuple->value->uint32);
      }
    
      break;
    
    case WEATHER_SUNSET_KEY:
      if ( new_tuple->value->uint32 > 0 ) {
        current_weather.sunset = new_tuple->value->uint32;
        persist_write_int(WEATHER_SUNSET_KEY, current_weather.sunset);
        
        APP_LOG(APP_LOG_LEVEL_DEBUG, "received sunset at: %u", (int) new_tuple->value->uint32);                
      }

      break;
    
    case KEY_VIBRATE:
      vibrate = new_tuple->value->uint32;
      persist_write_int(KEY_VIBRATE, vibrate);

      APP_LOG(APP_LOG_LEVEL_DEBUG, "vibrate set to %d", vibrate);
              
      break;
    
    case KEY_EUDATE:
      eudate = new_tuple->value->uint32;
      persist_write_int(KEY_EUDATE, eudate);
      layer_mark_dirty(s_date_layer);
    
      APP_LOG(APP_LOG_LEVEL_DEBUG, "eudate set to %d", eudate);
              
      break;
    
    case KEY_TOPBAR:
      topbar = new_tuple->value->uint32;
      persist_write_int(KEY_TOPBAR, topbar);
      if ( topbar == 0 ) {
        layer_set_hidden(s_bt_icon_layer, true);
        layer_set_hidden(s_bat_icon_layer, true);
      } else {
        layer_set_hidden(s_bt_icon_layer, false);
        layer_set_hidden(s_bat_icon_layer, false);
      }
   
      APP_LOG(APP_LOG_LEVEL_DEBUG, "topbar set to %d", topbar);
              
      break;
    
    case KEY_BOTTOMBAR:
      bottombar = new_tuple->value->uint32;
      persist_write_int(KEY_BOTTOMBAR, bottombar);
      if ( bottombar == 0 ) {
        layer_set_hidden(s_weather_layer, true);
      } else {
        layer_set_hidden(s_weather_layer, false);
      }

      APP_LOG(APP_LOG_LEVEL_DEBUG, "bottombar set to %d", bottombar);
              
      break;  
    
    case KEY_CELSIUS:
      celsius = new_tuple->value->uint32;
      persist_write_int(KEY_CELSIUS, celsius);
      //layer_mark_dirty(s_weather_layer);

      APP_LOG(APP_LOG_LEVEL_DEBUG, "celsius set to %d", celsius);
              
      break;
    
    case KEY_24HOUR:
      if ( new_tuple->value->uint32 == 1 ) {
        if ( notify_24hours_trigger == 1 ) {
          notify_24hours_trigger = 0;
          notify_24hours();
        }
      }
    
      break;
  }
}

static bool request_weather(void) {
  
  bool connected = connection_service_peek_pebble_app_connection();
  
  if ( bottombar == 1 && connected) {
    DictionaryIterator *iter;
    int result = app_message_outbox_begin(&iter);
    if ( result != APP_MSG_OK ) {
      return(false);
    }
  
    if (!iter) {
      // Error creating outbound message
      return(false);
    }
  
    int value = 1;
    dict_write_int(iter, 1, &value, sizeof(int), true);
    dict_write_end(iter);
  
    result = app_message_outbox_send();
    if ( result !=  APP_MSG_OK ) {
      return(false);
    }
    
    current_weather.last_try = (uint32_t) time(NULL);
  }
  
  return(true);
}

static void get_weather(void *data) {
  bool result = request_weather();
  if ( !result ) {
    APP_LOG(APP_LOG_LEVEL_INFO, "get_weather(): request failed, trying again in 250ms");
    retries++;
    if ( retries < 5 ) {
      app_timer_register(500, get_weather, NULL);
    } else {
      retries = 0;
    }
  } else {
    retries = 0;
  }
}

char *upcase(char *str) {
    char *s = str;

    while (*s) {
        *s++ = toupper((int)*s);
    }

    return str;
}

static void app_focus_changing(bool focusing) {
  if (focusing) {
     layer_set_hidden(window_get_root_layer(s_window), true);  
  }
}

static void app_focus_changed(bool focused) {
  if (focused) {
     layer_set_hidden(window_get_root_layer(s_window), false);  
     layer_mark_dirty(window_get_root_layer(s_window));
  }
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
  
  if ( (uint32_t) time(NULL) > current_weather.timestamp + REQUEST_WEATHER_INTERVAL_SECS ) {
    if ( (uint32_t) time(NULL) > current_weather.last_try + WEATHER_RETRY_INTERVAL_SECS ) {
      get_weather(NULL);
    }
  }
}

static void bluetooth_callback(bool connected) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "bt connected = %d", connected);
  
  layer_mark_dirty(s_bt_icon_layer);
  
  if ( first_run == 0 && vibrate == 1)
    vibes_double_pulse();
  
  if ( first_run == 1 )
    first_run = 0;
  
  if ( connected && (uint32_t) time(NULL) > current_weather.timestamp + REQUEST_WEATHER_INTERVAL_SECS ) {
    if ( (uint32_t) time(NULL) > current_weather.last_try + WEATHER_RETRY_INTERVAL_SECS ) {
      get_weather(NULL);
    }
  }
  
}

static void weather_update_proc(Layer *layer, GContext *ctx) {
 
#if defined(SCREENSHOT)
  current_weather.code = 800;
  current_weather.temperature = 68;
#endif 

  GRect bounds = layer_get_bounds(layer);
  
  if (s_weather_icon_bitmap) {
    gbitmap_destroy(s_weather_icon_bitmap);
    s_weather_icon_bitmap = NULL;
  }
  
  if ( current_weather.code > 0 && (uint32_t) time(NULL) < current_weather.timestamp + WEATHER_VALID_FOR_SECS ) {
            
    int item = 0;
    for(int i = 0; i < 83; ++i) {
      if ( WEATHER_DAY_ICONS_CODE_DEF[i].code == current_weather.code ) {
        item = i;
      }
    }
  
    uint32_t now = (uint32_t) time(NULL);
    WeatherIcons icon = WEATHER_DAY_ICONS_CODE_DEF[item];
    if ( now < current_weather.sunrise || now > current_weather.sunset ) {
      icon = WEATHER_NIGHT_ICONS_CODE_DEF[item];
      APP_LOG(APP_LOG_LEVEL_DEBUG, "using night icons");
    }
        
    uint16_t x = icon.x * 32;
    uint16_t y = WEATHER_ICON_SPRITE_LINES[icon.y];
    uint16_t height = WEATHER_ICON_SPRITE_LINES[icon.y+1] - WEATHER_ICON_SPRITE_LINES[icon.y];
    uint16_t width = 32;
    GRect area = { { x, y }, {width, height} };
      
    s_weather_icon_bitmap = gbitmap_create_as_sub_bitmap(s_weather_icons_bitmap, area);
    bitmap_layer_set_bitmap(s_weather_icon_bmlayer, s_weather_icon_bitmap);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "plotting code = %d, x=%d, y=%d", current_weather.code, icon.x, icon.y);
    
  } else {
    GRect area = { { WEATHER_DAY_ICONS_CODE_DEF[0].x * 32, WEATHER_ICON_SPRITE_LINES[WEATHER_DAY_ICONS_CODE_DEF[0].y]}, { 32, 24} };
    
    s_weather_icon_bitmap = gbitmap_create_as_sub_bitmap(s_weather_icons_bitmap, area);
    bitmap_layer_set_bitmap(s_weather_icon_bmlayer, s_weather_icon_bitmap);
  }
  
  if ( current_weather.temperature < 999 && (uint32_t) time(NULL) < current_weather.timestamp + WEATHER_VALID_FOR_SECS ) {
    
    for (int i = 0; i < 3; ++i) {
      if (s_temp_bmlayer[i]) {
        bitmap_layer_destroy(s_temp_bmlayer[i]);
        s_temp_bmlayer[i] = NULL;
      }
      if (s_temp_bitmap[i]) {
        gbitmap_destroy(s_temp_bitmap[i]);
        s_temp_bitmap[i] = NULL;
      }
    }
    
    int number;
    int x = bounds.size.w / 2 + 1;
    
    int16_t temperature = current_weather.temperature;
    if ( celsius == 1 ) {
      temperature = ( current_weather.temperature - 32 ) / 1.8;
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "temperature = %d (%d)", temperature, celsius);
    
    char temperature_label[24];
    snprintf(temperature_label, sizeof(temperature_label), "%d", temperature);
    for (uint8_t i = 0; i < strlen(temperature_label); ++i) {

      number = temperature_label[i];
      APP_LOG(APP_LOG_LEVEL_DEBUG, "temp number = %d", number);
    
      if ( number == 48 && i == 8 ) { // ignore leading zero in day of month
      } else if ( number == 32 ) { // space
        x = x + 6;
      } else {
        if ( number >= 48 && number <= 57 ) {
          number = number - 22;
        } else if ( number >= 65 && number <= 90) {
          number = number - 65;
        } else if ( number == 45 ) { // minus
          number = 36;
        } else if ( number == 167 ) { // degree symbol
          number = 37;
        }
        
        APP_LOG(APP_LOG_LEVEL_DEBUG, "temp sprite number = %d (%d)", number, x);
      
        s_temp_bitmap[i] = gbitmap_create_as_sub_bitmap(s_alphanumeric_sprite_bitmap, ALPANUMERIC_SPRITE_DEF[number]);
        s_temp_bmlayer[i] = bitmap_layer_create(GRect(x, UI_BOTTOM_BAR_Y+21, ALPANUMERIC_SPRITE_DEF[number].size.w, ALPANUMERIC_SPRITE_DEF[number].size.h));
        
        bitmap_layer_set_bitmap(s_temp_bmlayer[i], s_temp_bitmap[i]);
        layer_add_child(layer, bitmap_layer_get_layer(s_temp_bmlayer[i]));
      
        x = x + ALPANUMERIC_SPRITE_DEF[number].size.w + 1;        
      }
    }
    
    text_layer_set_text(s_temperature_layer, "");    
  } else {
    
    for (int i = 0; i < 3; ++i) {
      if (s_temp_bmlayer[i]) {
        bitmap_layer_destroy(s_temp_bmlayer[i]);
        s_temp_bmlayer[i] = NULL;
      }
      if (s_temp_bitmap[i]) {
        gbitmap_destroy(s_temp_bitmap[i]);
        s_temp_bitmap[i] = NULL;
      }
    }
    
    text_layer_set_text(s_temperature_layer, "- -");
  }  
}

static void bat_update_proc(Layer *layer, GContext *ctx) {
  if ( topbar == 0 ) {
    layer_set_hidden(layer, true);
  } else {
    layer_set_hidden(layer, false);
    BatteryChargeState bat = battery_state_service_peek();
    
#ifdef PBL_ROUND
    int x = 93;
#else
    int x = 113;
#endif
    
    if ( bat.is_charging && bat.charge_percent > bat_charge ) {
      bat_charge = bat.charge_percent;
    } 
    if ( ! bat.is_charging ) {
      bat_charge = bat.charge_percent;    
    }
    
    if ( bat_charge > 10 ) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    } else {
      graphics_context_set_stroke_color(ctx, GColorRed);
      graphics_context_set_fill_color(ctx, GColorRed);
    }
    graphics_draw_rect(ctx, GRect(x, UI_TOP_BAR_Y + 4, 18, 9));
    graphics_fill_rect(ctx, GRect(x+18, UI_TOP_BAR_Y + 7, 2, 3), 0, GCornerNone);
    
    int width = bat_charge * 16 / 100;
  //  APP_LOG(APP_LOG_LEVEL_DEBUG, "bat width = %d", width);
    if ( bat_charge > 10 )
    {
      graphics_context_set_fill_color(ctx, GColorBrightGreen);
    } else {
      graphics_context_set_fill_color(ctx, GColorRed);
    }
    graphics_fill_rect(ctx, GRect(x+1, UI_TOP_BAR_Y + 5, width, 7),  0, GCornerNone);
    
    if (bat.is_charging) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
      gpath_draw_outline(ctx, s_charging_icon_gpath);
      gpath_draw_filled(ctx, s_charging_icon_gpath);
    }
  }
}

static void bt_update_proc(Layer *layer, GContext *ctx) {
   if ( topbar == 0 ) {
    layer_set_hidden(layer, true);
  } else {
    layer_set_hidden(layer, false);
    bool connected = connection_service_peek_pebble_app_connection();
    if(connected)
    {
      graphics_context_set_stroke_color(ctx, GColorWhite);
    } else {
      graphics_context_set_stroke_color(ctx, GColorRed);
    }
    gpath_draw_outline_open(ctx, s_bt_icon_gpath);
  }
}

static void bg_update_proc(Layer *layer, GContext *ctx) {

#if defined(DARK_VERSION)
  graphics_context_set_fill_color(ctx, GColorBlack);
#else
  graphics_context_set_fill_color(ctx, GColorWhite);
#endif
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void time_update_proc(Layer *layer, GContext *ctx) {

  GRect bounds = layer_get_bounds(layer);
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  // for screenshot purposes (10:09.30)
#if defined(SCREENSHOT)
  t->tm_sec = 30;
  t->tm_min = 9; 
  t->tm_hour = 10;
#endif
  
  if ( clock_is_24h_style() )
  {
      strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", t);
  } else {
      strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M", t);
  }
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "time = %s", s_time_buffer);
  
  for (int i = 0; i < 5; ++i) {
    if ( s_number_bitmap[i] ) {
      gbitmap_destroy(s_number_bitmap[i]);
      s_number_bitmap[i] = NULL;
    }
    if ( s_number_layer[i] ) {
      bitmap_layer_destroy(s_number_layer[i]);
      s_number_layer[i] = NULL;
    }
  }
  layer_remove_child_layers(layer);
  
  int number;
  
  int width = 0;
  for (int i = 0; i < 5; ++i) {

    number = s_time_buffer[i];
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "number = %d", number);
    
    if ( number == 48 && i == 0 ) {
    } else {

      if ( number == 58 ) {
        number = 10;
      } else {
        if ( number == 48 )
        {
          number = 9;
        } else {
          number = number - 49;
        }
      }
      
      width = width + NUMBER_SPRITE_DEF[number].size.w + 5;
    }
  }
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "width = %d", width);

  int x = (bounds.size.w - (width - 5)) / 2;
  
  for (int i = 0; i < 5; ++i) {

    number = s_time_buffer[i];
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "number = %d", number);
    
    if ( number == 48 && i == 0 ) {
    } else {

      if ( number == 58 ) {
        number = 10;
      } else {
        if ( number == 48 )
        {
          number = 9;
        } else {
          number = number - 49;
        }
      }
      
 //     APP_LOG(APP_LOG_LEVEL_DEBUG, "sprite number = %d", number);
      
      s_number_bitmap[i] = gbitmap_create_as_sub_bitmap(s_numbers_sprite_bitmap, NUMBER_SPRITE_DEF[number]);
      s_number_layer[i] = bitmap_layer_create(GRect(x, UI_TIME_BAR_Y, NUMBER_SPRITE_DEF[number].size.w, NUMBER_SPRITE_DEF[number].size.h));
        
      bitmap_layer_set_bitmap(s_number_layer[i], s_number_bitmap[i]);
      layer_add_child (layer, bitmap_layer_get_layer(s_number_layer[i]));
      
      x = x + NUMBER_SPRITE_DEF[number].size.w + 5;
    }
  }
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  
  GRect bounds = layer_get_bounds(layer);
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  // for screenshot purposes (Tue Sep 9)
#if defined(SCREENSHOT)
  t->tm_mday = 9;
  t->tm_mon = 8;
  t->tm_wday = 2;
#endif
  
  if ( eudate == 0 ) {
    strftime(s_date_buffer, sizeof(s_date_buffer), "%a %b %d", t);
  } else {
    strftime(s_date_buffer, sizeof(s_date_buffer), "%a %d %b", t);
  }
  upcase(s_date_buffer);
  
  for (int i = 0; i < 10; ++i) {
    if ( s_label_bitmap[i] ) {
      gbitmap_destroy(s_label_bitmap[i]);
      s_label_bitmap[i] = NULL;
    }
    if ( s_label_layer[i] ) {
      bitmap_layer_destroy(s_label_layer[i]);
      s_label_layer[i] = NULL;
    }
  }
  layer_remove_child_layers(layer);
    
  int number;
  
  int width = 0;
  for (int i = 0; i < 10; ++i) {

    number = s_date_buffer[i];
    
    if ( number == 48 && i == 8 ) { // ignore leading zero in day of month
    } else if ( number == 32 ) { // space
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "date space = %d (%d)", number, width);
      width = width + 6;
    } else {
      if ( number >= 48 && number <= 57 ) {
        number = number - 22;
      } else if ( number >= 65 && number <= 90) {
        number = number - 65;
      }
      
      width = width + ALPANUMERIC_SPRITE_DEF[number].size.w + 1;
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "date number = %d (width = %d, size = %d)", number, width,  ALPANUMERIC_SPRITE_DEF[number].size.w);
    }
  }
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "date width = %d", width);
  
  int x = (bounds.size.w - width) / 2;
  
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "date start x = %d", x);

  
  for (int i = 0; i < 10; ++i) {

    number = s_date_buffer[i];
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "date number = %d", number);
    
    if ( number == 48 && i == 8 ) { // ignore leading zero in day of month
    } else if ( number == 32 ) { // space
      x = x + 6;
    } else {
      if ( number >= 48 && number <= 57 ) {
        number = number - 22;
      } else if ( number >= 65 && number <= 90) {
        number = number - 65;
      }
      
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "date sprite number = %d (%d)", number, x);
      
      s_label_bitmap[i] = gbitmap_create_as_sub_bitmap(s_alphanumeric_sprite_bitmap, ALPANUMERIC_SPRITE_DEF[number]);
      s_label_layer[i] = bitmap_layer_create(GRect(x, UI_DATE_BAR_Y, ALPANUMERIC_SPRITE_DEF[number].size.w, ALPANUMERIC_SPRITE_DEF[number].size.h));
        
      bitmap_layer_set_bitmap(s_label_layer[i], s_label_bitmap[i]);
      layer_add_child(layer, bitmap_layer_get_layer(s_label_layer[i]));
      
      x = x + ALPANUMERIC_SPRITE_DEF[number].size.w + 1;
    }
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);
  
  s_time_layer = layer_create(bounds);
  layer_set_update_proc(s_time_layer, time_update_proc);
  layer_add_child(window_layer, s_time_layer);

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);
  
  s_bt_icon_layer = layer_create(bounds);
  layer_set_update_proc(s_bt_icon_layer, bt_update_proc);
  layer_add_child(window_layer, s_bt_icon_layer);
  
  s_bat_icon_layer = layer_create(bounds);
  layer_set_update_proc(s_bat_icon_layer, bat_update_proc);
  layer_add_child(window_layer, s_bat_icon_layer);
  
  s_weather_layer = layer_create(bounds);
  layer_set_update_proc(s_weather_layer, weather_update_proc);
  layer_add_child(window_layer, s_weather_layer);
  int x = ( bounds.size.w / 2 ) - 34;
  s_weather_icon_bmlayer = bitmap_layer_create(GRect(x, UI_BOTTOM_BAR_Y, 40, 57));
  layer_add_child(s_weather_layer, bitmap_layer_get_layer(s_weather_icon_bmlayer));
  x = ( bounds.size.w / 2 ) + 2;
  s_temperature_layer = text_layer_create(GRect(x, UI_BOTTOM_BAR_Y+16, 32, 27));
  text_layer_set_text_color(s_temperature_layer, GColorWhite);
  text_layer_set_background_color(s_temperature_layer, GColorClear);
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentLeft);
  layer_add_child(s_weather_layer, text_layer_get_layer(s_temperature_layer));

  Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, current_weather.code),
    TupletInteger(WEATHER_TEMPERATURE_KEY, current_weather.temperature),
    TupletInteger(WEATHER_SUNRISE_KEY, current_weather.sunrise),
    TupletInteger(WEATHER_SUNSET_KEY, current_weather.sunset),
    TupletInteger(WEATHER_TIMESTAMP_KEY, current_weather.timestamp),
    TupletInteger(KEY_VIBRATE, vibrate),
    TupletInteger(KEY_EUDATE, eudate),
    TupletInteger(KEY_TOPBAR, topbar),
    TupletInteger(KEY_BOTTOMBAR, bottombar),
    TupletInteger(KEY_CELSIUS, celsius),
    TupletInteger(KEY_24HOUR, 0)
  };

  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer),
      initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);
  
  if ( (uint32_t) time(NULL) > current_weather.timestamp + REQUEST_WEATHER_INTERVAL_SECS ) {
    get_weather(NULL);
  }
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_time_layer);
  layer_destroy(s_date_layer);
}

static void init() {
  
  if ( persist_exists(KEY_VIBRATE) )
    vibrate = persist_read_int(KEY_VIBRATE);
  if ( persist_exists(KEY_EUDATE) ) {
    eudate = persist_read_int(KEY_EUDATE);
  } else {
    if ( clock_is_24h_style() ) {
      eudate = 1;
      notify_24hours_trigger = 1;
    }
  }
  if ( persist_exists(KEY_TOPBAR) )
    topbar = persist_read_int(KEY_TOPBAR);
  if ( persist_exists(KEY_BOTTOMBAR) )
    bottombar = persist_read_int(KEY_BOTTOMBAR);
  if ( persist_exists(KEY_CELSIUS) ) {
    celsius = persist_read_int(KEY_CELSIUS);
  } else {
    if ( clock_is_24h_style() ) {
      celsius = 1;
    }
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "READ from memory: vibrate = %d, eudate = %d, celsius = %d", vibrate, eudate, celsius);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "READ from memory: topbar = %d, bottombar = %d", topbar, bottombar);  

  if ( persist_exists(WEATHER_ICON_KEY) )
    current_weather.code = persist_read_int(WEATHER_ICON_KEY);
  if ( persist_exists(WEATHER_TEMPERATURE_KEY) )
    current_weather.temperature = persist_read_int(WEATHER_TEMPERATURE_KEY);
  if ( persist_exists(WEATHER_SUNRISE_KEY) )
    current_weather.sunrise = persist_read_int(WEATHER_SUNRISE_KEY);
  if ( persist_exists(WEATHER_SUNSET_KEY) )
    current_weather.sunset = persist_read_int(WEATHER_SUNSET_KEY);
  if ( persist_exists(WEATHER_TIMESTAMP_KEY))
    current_weather.timestamp = persist_read_int(WEATHER_TIMESTAMP_KEY);  
      
  APP_LOG(APP_LOG_LEVEL_DEBUG, "READ from memory: icon = %d, temperature = %d, ts = %u", current_weather.code, current_weather.temperature, (int) current_weather.timestamp);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "READ from memory: sunrise = %u, sunset = %u", (int) current_weather.sunrise, (int) current_weather.sunset);
  
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  
  s_numbers_sprite_bitmap = gbitmap_create_with_resource(RESOURCE_ID_roboto_thin_64_numberic_plus_colon);
  s_weather_icons_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_SPRITE_25PX);
  s_alphanumeric_sprite_bitmap = gbitmap_create_with_resource(RESOURCE_ID_roboto_thin_20_alphanumberic);
    
  s_bt_icon_gpath = gpath_create(&BT_ICON_PATH);
#ifdef PBL_ROUND
  gpath_move_to(s_bt_icon_gpath, GPoint(72,UI_TOP_BAR_Y)); 
#else
  gpath_move_to(s_bt_icon_gpath, GPoint(92,UI_TOP_BAR_Y));
#endif
  
  s_charging_icon_gpath = gpath_create(&CHARGING_ICON_PATH);
#ifdef PBL_ROUND
  gpath_move_to(s_charging_icon_gpath, GPoint(115,UI_TOP_BAR_Y+3)); 
#else
  gpath_move_to(s_charging_icon_gpath, GPoint(135,UI_TOP_BAR_Y+3)); 
#endif
  tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);  
  
  battery_state_service_subscribe(battery_handler);
  
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
  bluetooth_callback(connection_service_peek_pebble_app_connection());

  app_message_open(256, 256);
  
  // WORKAROUND: PTR sometimes fails to redraw layers on return / gain focus - force redraw
  app_focus_service_subscribe_handlers((AppFocusHandlers){
    .did_focus = app_focus_changed,
    .will_focus = app_focus_changing
  });
}

static void deinit() {
  tick_timer_service_unsubscribe();
  
  if ( s_weather_icon_bitmap ) {
    gbitmap_destroy(s_weather_icon_bitmap);
  }
  
  for (int i = 0; i < 10; ++i) {
    if ( s_label_bitmap[i] ) {
      gbitmap_destroy(s_label_bitmap[i]);
    }
  }
  
  for (int i = 0; i < 5; ++i) {
    if ( s_number_bitmap[i] ) {
      gbitmap_destroy(s_number_bitmap[i]);
    }
  }
  
  for (int i = 0; i < 3; ++i) {
    if (s_temp_bitmap[i]) {
      gbitmap_destroy(s_temp_bitmap[i]);
    }
  }
  
  for (int i = 0; i < 5; ++i) {
    if (s_number_layer[i]) {
      bitmap_layer_destroy(s_number_layer[i]);
    }
  }
  
  for (int i = 0; i < 10; ++i) {
    if (s_label_layer[i]) {
      bitmap_layer_destroy(s_label_layer[i]);
    }
  }

  for (int i = 0; i < 4; ++i) {
    if (s_temp_bmlayer[i]) {
      bitmap_layer_destroy(s_temp_bmlayer[i]);
    }
  }
  
  gbitmap_destroy(s_numbers_sprite_bitmap);
  gbitmap_destroy(s_weather_icons_bitmap);
  gbitmap_destroy(s_alphanumeric_sprite_bitmap);
    
  bitmap_layer_destroy(s_weather_icon_bmlayer);
  text_layer_destroy(s_temperature_layer);
  
  gpath_destroy(s_bt_icon_gpath);
  gpath_destroy(s_charging_icon_gpath);
  
  layer_destroy(s_bt_icon_layer);
  layer_destroy(s_bat_icon_layer);
  layer_destroy(s_weather_layer);
  
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
