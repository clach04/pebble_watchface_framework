#pragma once

#include "watch_config.h"
    
/* PebbleKit JS, Message Keys, Pebble config keys */
// FIXME why can't this be generated from the json settings file into a header?
#define KEY_TIME_COLOR 0
#define KEY_VIBRATE_ON_DISCONNECT 1
#define KEY_BACKGROUND_COLOR 2

#ifndef BLUETOOTH_DISCONNECTED_STR
#define BLUETOOTH_DISCONNECTED_STR "BT Disconnected"
#endif /* BLUETOOTH_DISCONNECTED_STR */

#ifndef DATE_FMT_STR
#define DATE_FMT_STR "%a, %d %b"  /* TODO review %d for day */
#endif /* DATE_FMT_STR */
#define MAX_DATE_STR "Thu, 00 Aug"

#define MAX_TIME_STR "00:00"

#ifndef BAT_FMT_STR
#define BAT_FMT_STR "Bat: %d%%"
#endif /* BAT_FMT_STR */
#define MAX_BAT_STR "Bat: ??%"  // When Battery is 100, the percent symbol is deliberately not shown (buffer full/truncated)

#ifndef FONT_BT_SYSTEM_NAME
#define FONT_BT_SYSTEM_NAME FONT_KEY_GOTHIC_18
#endif /* FONT_BT_SYSTEM_NAME */

#ifndef FONT_DATE_SYSTEM_NAME
#define FONT_DATE_SYSTEM_NAME FONT_KEY_GOTHIC_18_BOLD
#endif /* FONT_DATE_SYSTEM_NAME */

#ifndef FONT_BAT_SYSTEM_NAME
#define FONT_BAT_SYSTEM_NAME FONT_KEY_GOTHIC_18
#endif /* FONT_BAT_SYSTEM_NAME */

#ifndef FONT_SYSTEM_NAME
/*
** Also consider FONT_KEY_BITHAM_42_BOLD, FONT_KEY_ROBOTO_CONDENSED_21
*/
#define FONT_SYSTEM_NAME FONT_KEY_ROBOTO_BOLD_SUBSET_49
#endif /* FONT_SYSTEM_NAME */

#ifndef BT_ALIGN
#define BT_ALIGN GTextAlignmentCenter
#endif /* BT_ALIGN */

#ifndef BAT_ALIGN
#define BAT_ALIGN GTextAlignmentLeft
#endif /* BAT_ALIGN */

#ifndef TIME_ALIGN
#define TIME_ALIGN GTextAlignmentCenter
#endif /* TIME_ALIGN */

#ifndef DEFAULT_TIME_COLOR
#define DEFAULT_TIME_COLOR GColorWhite
#endif /* DEFAULT_TIME_COLOR */

#ifndef DEFAULT_BACKGROUND_COLOR
#define DEFAULT_BACKGROUND_COLOR GColorBlack
#endif /* DEFAULT_BACKGROUND_COLOR */

#ifndef TICK_HANDLER
#define TICK_HANDLER tick_handler
#endif /* TICK_HANDLER */

#ifndef MAIN_WINDOW_LOAD
#define MAIN_WINDOW_LOAD main_window_load
#endif /* MAIN_WINDOW_LOAD */

#ifndef MAIN_WINDOW_UNLOAD
#define MAIN_WINDOW_UNLOAD main_window_unload
#endif /* MAIN_WINDOW_UNLOAD */

#ifndef BG_IMAGE_GRECT
#define BG_IMAGE_GRECT GRectZero
#endif /* BG_IMAGE_GRECT */


extern Window    *main_window;
extern TextLayer *time_layer;
extern TextLayer *date_layer;
extern TextLayer *battery_layer;
extern TextLayer *bluetooth_layer;

extern GFont       time_font;
extern BitmapLayer *background_layer;
extern GBitmap     *background_bitmap;
/* For colors, see http://developer.getpebble.com/tools/color-picker/#0000FF */
extern GColor       time_color;  /* NOTE used for date too */
extern GColor       background_color;
extern int          config_time_color;
extern int          config_background_color;
extern bool         config_time_vib_on_disconnect;

extern int last_day;
extern bool bluetooth_state;

extern void handle_bluetooth(bool connected);
extern void setup_bluetooth(Window *window);
extern void cleanup_bluetooth();
extern void handle_battery(BatteryChargeState charge_state);
extern void setup_battery(Window *window);
extern void cleanup_battery();
extern void update_date(struct tm *tick_time);
extern void setup_date(Window *window);
extern void cleanup_date();
extern void update_time();
extern void main_window_load(Window *window);
extern void main_window_unload(Window *window);
extern void tick_handler(struct tm *tick_time, TimeUnits units_changed);
extern void in_recv_handler(DictionaryIterator *iterator, void *context);
extern void init();
extern void deinit();
