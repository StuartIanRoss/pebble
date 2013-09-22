#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x7C, 0x41, 0x55, 0x92, 0x39, 0xAF, 0x48, 0x64, 0xBC, 0xA3, 0x6F, 0x6A, 0x0B, 0x9E, 0x38, 0x4E }
PBL_APP_INFO(MY_UUID,
             "Test App", "Stuart Ross",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer hello_layer;

TextLayer text_date_layer;
TextLayer text_time_layer;

Layer line_layer;

void line_layer_update_callback(Layer *me, GContext* ctx) {

  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_draw_line(ctx, GPoint(8, 97), GPoint(131, 97));
  graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));

}

void handle_init(AppContextRef ctx) {

  window_init(&window, "Window Name");
  window_set_fullscreen(&window, true);
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
  
  text_layer_init(&hello_layer, window.layer.frame);
  text_layer_set_text_alignment(&hello_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&hello_layer, GColorWhite);
  text_layer_set_background_color(&hello_layer, GColorClear);
  text_layer_set_text(&hello_layer, "It's PEW O'Clock");
  layer_set_frame(&hello_layer.layer, GRect(2, 10, 144-2, 168-10));
  text_layer_set_font(&hello_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21)); 
  layer_add_child(&window.layer, &hello_layer.layer);
  
  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(8, 68, 144-8, 168-68));
  text_layer_set_font(&text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(&window.layer, &text_date_layer.layer);


  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(7, 92, 144-7, 168-92));
  text_layer_set_font(&text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(&window.layer, &text_time_layer.layer);


  layer_init(&line_layer, window.layer.frame);
  line_layer.update_proc = &line_layer_update_callback;
  layer_add_child(&window.layer, &line_layer);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";

  char *time_format;


  // TODO: Only update the date when it's changed.
  string_format_time(date_text, sizeof(date_text), "%B %e", t->tick_time);
  text_layer_set_text(&text_date_layer, date_text);


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(&text_time_layer, time_text);

}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
