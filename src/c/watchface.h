#pragma once

#include "watch_config.h"

#ifndef USE_MAX_MESSAGE_SIZE
    /*
    ** framework expects ONLY the three items above
    ** pebble-js-app.js sanitizes values and
    ** max length is about 90 bytes incoming, e.g.:
    **    {"KEY_TIME_COLOR":16777215,"KEY_BACKGROUND_COLOR":16777215,"KEY_VIBRATE_ON_DISCONNECT":0}
    ** Setting accurate max sizes avoids warnings:
    **  [INFO] essage_outbox.c:49: app_message_open() called with app_message_outbox_size_maximum().
    **  [INFO] essage_outbox.c:52: This consumes 8200 bytes of heap memory, potentially more in the future!
    **  [INFO] message_inbox.c:13: app_message_open() called with app_message_inbox_size_maximum().
    **  [INFO] message_inbox.c:16: This consumes 8200 bytes of heap memory, potentially more in the future!    
    */
    #define MAX_MESSAGE_SIZES
    #define MAX_MESSAGE_SIZE_IN 200
    #define MAX_MESSAGE_SIZE_OUT 0
#endif

#ifndef BLUETOOTH_DISCONNECTED_STR
#define BLUETOOTH_DISCONNECTED_STR "BT Disconnected"
#endif /* BLUETOOTH_DISCONNECTED_STR */

#ifndef DATE_FMT_STR
/*
** DATE_FMT_STR format for date, passed to strftime()
** https://developer.getpebble.com/docs/c/Standard_C/Time/#strftime
** See http://www.gnu.org/software/emacs/manual/html_node/elisp/Time-Parsing.html for more details
*/
#define DATE_FMT_STR "%a, %d %b"  /* TODO review %d for day */
#define MAX_DATE_STR "Thu, 00 Aug"  /* if custom version of DATE_FMT_STR is set, MAX_DATE_STR  needs to be updated too */
#endif /* DATE_FMT_STR */

#ifndef TIME_FMT_STR_12H
    /*
    ** See https://sourceware.org/newlib/libc.html#strftime
    ** Assume 24h not set either
    ** Examples:
    **    24 Hour: "%H:%M" == "%R"
    **    12 Hour: "%l:%M" NOTE leading space for hour. NOTE requires font to have a space character
    **             Also see REMOVE_LEADING_ZERO_FROM_TIME
    */
    #define TIME_FMT_STR_24H "%R"
    #define TIME_FMT_STR_12H "%I:%M"  // produces leading zero for both hour and minute
    #define MAX_TIME_STR "00:00"
#endif /* TIME_FMT_STR_24H */

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

#ifndef DATE_ALIGN
#define DATE_ALIGN GTextAlignmentRight
#endif /* TIME_ALIGN */

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

#ifndef TICK_HANDLER_INTERVAL
#define TICK_HANDLER_INTERVAL MINUTE_UNIT
#endif /* TICK_HANDLER_INTERVAL */

#ifndef DEBUG_TICK_HANDLER
#define DEBUG_TICK_HANDLER debug_tick_handler
#endif /* DEBUG_TICK_HANDLER */

#ifndef SETUP_TIME
#define SETUP_TIME setup_text_time
#endif /* SETUP_TIME */

#ifndef CLEANUP_TIME
#define CLEANUP_TIME cleanup_text_time
#endif /* CLEANUP_TIME */

#ifndef MAIN_WINDOW_LOAD
#define MAIN_WINDOW_LOAD main_window_load
#endif /* MAIN_WINDOW_LOAD */

#ifndef MAIN_WINDOW_UNLOAD
#define MAIN_WINDOW_UNLOAD main_window_unload
#endif /* MAIN_WINDOW_UNLOAD */

extern Window    *main_window;
#ifndef NO_TEXT_TIME_LAYER
extern TextLayer *time_layer;
#endif /* NO_TEXT_TIME_LAYER */

extern TextLayer *date_layer;
#ifndef DRAW_BATTERY
extern TextLayer *battery_layer;
#else
extern Layer *battery_layer;
#endif /* DRAW_BATTERY */
extern TextLayer *bluetooth_tlayer;

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
extern void in_recv_handler(DictionaryIterator *iterator, void *context);
extern void init();
extern void deinit();
extern void TICK_HANDLER(struct tm *tick_time, TimeUnits units_changed);

extern void CLEANUP_TIME();
extern void SETUP_TIME(Window *window);

/*
** CUSTOM_IN_RECV_HANDLER() like app_message_register_inbox_received()
** but returns bool flag if config was written to persistent storage
*/
extern bool CUSTOM_IN_RECV_HANDLER(DictionaryIterator *iterator, void *context);

#ifndef PBL_HEALTH
    // Ensure platforms without health do not accidentally attempt to build using health api
    #undef USE_HEALTH
#endif /* PBL_HEALTH */

#if defined(PBL_HEALTH)
extern TextLayer *health_tlayer;

#ifndef HEALTH_ALIGN
    #define HEALTH_ALIGN GTextAlignmentCenter
#endif

#ifndef HEALTH_FMT_STR
    #define HEALTH_FMT_STR "%d steps today"
    #define MAX_HEALTH_STR "123456 steps today"   /* update to match HEALTH_FMT_STR */
#endif /* HEALTH_FMT_STR */

#ifndef HEALTH_POS
    #ifndef USE_HEALTH
        #define HEALTH_POS GRect(50, 50, 144, 168)
    #endif /* USE_HEALTH */
#endif /* HEALTH_POS */

void setup_health(Window *window);
void update_health();
void cleanup_health();
#endif /* PBL_HEALTH */
