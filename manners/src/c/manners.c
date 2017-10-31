#include <pebble.h>
#include <time.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_btc_layer;
static int s_battery_level;
static char s_btc_price_buffer[12];
static char last_btc[8];

static uint8_t greater_than(char* num1, char* num2) {     
  uint8_t is_greater = 0;
  uint8_t idx;
  char c_num1;
  char c_num2;
  for(idx=0; idx < sizeof(last_btc)-1; idx++) {
    // APP_LOG(APP_LOG_LEVEL_INFO, "new: %c, last: %c", (char)*num1, (char)*num2);
    c_num1 = (char)*num1++;
    c_num2 = (char)*num2++;
    if(c_num1 > c_num2) { 
      return 1;
    }
    else if(c_num1 < c_num2) {
      return 2;
    }
  }
  return is_greater;
}

static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  static char s_buffer[4];
  snprintf(s_buffer, sizeof(s_buffer), "%d", s_battery_level);
  text_layer_set_text(s_battery_layer, s_buffer);
}

static void refresh_web_data() {
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  
  static char s_buffer2[10];
  strftime(s_buffer2, sizeof(s_buffer2), "%m-%d", tick_time);
  

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, s_buffer2);
}

static void update_btc() {
  text_layer_set_text(s_btc_layer, s_btc_price_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  refresh_web_data();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_time_layer = text_layer_create(GRect(0, 52, bounds.size.w, 50));
  s_date_layer = text_layer_create(GRect(0, 0, bounds.size.w/2, 50));
  s_battery_layer = text_layer_create(GRect(bounds.size.w/2, 0, bounds.size.w/2, 50));
  s_btc_layer = text_layer_create(GRect(0, bounds.size.h - 25, bounds.size.w, 50));

  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "00-00");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorBlack);
  text_layer_set_text(s_battery_layer, "100");
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_btc_layer, GColorClear);
  text_layer_set_text_color(s_btc_layer, GColorBlack);
  text_layer_set_text(s_btc_layer, "BTC:?");
  text_layer_set_font(s_btc_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(s_btc_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_btc_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_btc_layer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read tuples for data
  Tuple *btc_tuple = dict_find(iterator, MESSAGE_KEY_BTC_PRICE);
  uint8_t btc_increasing = 0;

  // If all data is available, use it
  if(btc_tuple) {
    memcpy(last_btc, s_btc_price_buffer+4, 8);
    snprintf(s_btc_price_buffer, sizeof(s_btc_price_buffer), "BTC:%s", btc_tuple->value->cstring);
    btc_increasing = greater_than(s_btc_price_buffer+4, last_btc);
    // APP_LOG(APP_LOG_LEVEL_INFO, "new price: %s, last price: %s", s_btc_price_buffer, last_btc); 
    // APP_LOG(APP_LOG_LEVEL_INFO, "btc_increasing: %d", btc_increasing);        
    if(btc_increasing == 1) {
      s_btc_price_buffer[11] = '+';
    }
    else if(btc_increasing == 2) {
      s_btc_price_buffer[11] = '-';
    }
    else {
      s_btc_price_buffer[11] = '=';
    }
    update_btc();
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  const int inbox_size = 128;
  const int outbox_size = 128;

  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());  

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  // Open AppMessage
  app_message_open(inbox_size, outbox_size);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
