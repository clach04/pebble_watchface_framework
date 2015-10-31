#include <pebble.h>
#include "watchface.h"

#ifdef USE_SHADOW_TIME_EFFECT
#include "effect_layer.h"  /* from https://github.com/ygalanter/EffectLayer */
#endif /* USE_SHADOW_TIME_EFFECT */


Window    *main_window=NULL;
TextLayer *time_layer=NULL;
TextLayer *date_layer=NULL;
TextLayer *battery_layer=NULL;
TextLayer *bluetooth_layer=NULL;

GFont       time_font;
#ifdef BG_IMAGE
BitmapLayer *background_layer=NULL;
GBitmap     *background_bitmap=NULL;
#endif /* BG_IMAGE */
/* For colors, see http://developer.getpebble.com/tools/color-picker/#0000FF */
GColor       time_color;  /* NOTE used for date too */
GColor       background_color;
int          config_time_color;
int          config_background_color;
bool         config_time_vib_on_disconnect = false;

int last_day = -1;
bool bluetooth_state = false;

#ifdef DEBUG_TIME
char * debug_time_list[] = {
    "00:00",
    "01:01",
    "01:23",
    "04:56",
    "07:58",
    "07:59",
    "05:26",
    "07:48",
    "09:23",
    "11:11",
    "12:34",
    "22:22",
    "23:50",
    "23:55",
    /* 12 hour format withOUT leading zero, or space */
    "12:12",
    "1:01",
    "8:55",
    /* 12 hour format withOUT leading zero, with  space */
    " 1:01",
    " 8:55",
    NULL
};
#endif /* DEBUG_TIME */

#ifdef USE_SHADOW_TIME_EFFECT
static EffectLayer* effect_layer=NULL;
static EffectOffset effect_offset;


void setup_effects(Window *window)
{
    //effect_layer = effect_layer_create(GRect(0, 0, 144, 168)); // will cover entire screen
    effect_layer = effect_layer_create(CLOCK_POS);

    effect_offset.orig_color = time_color;
    effect_offset.offset_color = background_color;
    effect_offset.offset_y = 2;
    effect_offset.offset_x = 2;

    effect_layer_add_effect(effect_layer, effect_outline, &effect_offset);
    /* Other effect examples */
    //effect_layer_add_effect(effect_layer, effect_shadow, &effect_offset);

    //effect_layer_add_effect(effect_layer, effect_invert, NULL);
    //effect_layer_add_effect(effect_layer, effect_invert_bw_only, NULL);
    //effect_layer_add_effect(effect_layer, effect_rotate_90_degrees, (void *)true); // rotates 90 degrees counterclockwise

    layer_add_child(window_get_root_layer(window), effect_layer_get_layer(effect_layer));
}

void cleanup_effects()
{
    if (effect_layer)
    {
        effect_layer_destroy(effect_layer);
    }
}
#endif /* USE_SHADOW_TIME_EFFECT */

void handle_bluetooth(bool connected)
{
    /* TODO use gfx not text */
    if (connected)
    {
        text_layer_set_text(bluetooth_layer, "");
    }
    else
    {
        text_layer_set_text(bluetooth_layer, BLUETOOTH_DISCONNECTED_STR);
        if (config_time_vib_on_disconnect && (bluetooth_state != connected))
        {
            /* had BT connection then lost it, rather than started disconnected */
            vibes_short_pulse();  /* vibrate/rumble */
        }
    }
    bluetooth_state = connected;
}


void setup_bluetooth(Window *window)
{
    bluetooth_layer = text_layer_create(BT_POS);
    text_layer_set_text_color(bluetooth_layer, time_color);
    text_layer_set_background_color(bluetooth_layer, GColorClear);
    text_layer_set_font(bluetooth_layer, fonts_get_system_font(FONT_BT_SYSTEM_NAME));
    text_layer_set_text_alignment(bluetooth_layer, BT_ALIGN);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(bluetooth_layer));
    text_layer_set_text(bluetooth_layer, "");

    handle_bluetooth(bluetooth_connection_service_peek());
    bluetooth_connection_service_subscribe(handle_bluetooth);
}

void cleanup_bluetooth()
{
    bluetooth_connection_service_unsubscribe();
    text_layer_destroy(bluetooth_layer);
}

void handle_battery(BatteryChargeState charge_state) {
    static char battery_text[] = MAX_BAT_STR;

    if (charge_state.is_charging) {
        snprintf(battery_text, sizeof(battery_text), "Charging");
        text_layer_set_text_color(battery_layer, COLOR_FALLBACK(GColorGreen, time_color));
    } else {
        snprintf(battery_text, sizeof(battery_text), BAT_FMT_STR, charge_state.charge_percent);
#ifdef PBL_COLOR
        /* TODO Check charge level and change color? E.g. red at 10%/20% */
        if (charge_state.charge_percent <= 20)
        {
            text_layer_set_text_color(battery_layer, GColorRed);
        }
        else /* TODO different colors for different ranges */
        {
            /* TODO is this an expensive call ? */
            text_layer_set_text_color(battery_layer, time_color);
        }
#endif
    }
    text_layer_set_text(battery_layer, battery_text);
}

/* Battery level */
void setup_battery(Window *window)
{
    battery_layer = text_layer_create(BAT_POS);
    text_layer_set_text_color(battery_layer, time_color);
    text_layer_set_background_color(battery_layer, GColorClear);
    text_layer_set_font(battery_layer, fonts_get_system_font(FONT_BAT_SYSTEM_NAME));
    text_layer_set_text_alignment(battery_layer, BAT_ALIGN);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(battery_layer));
    text_layer_set_text(battery_layer, MAX_BAT_STR);

    battery_state_service_subscribe(handle_battery);
}

void cleanup_battery()
{
    battery_state_service_unsubscribe();
    text_layer_destroy(battery_layer);
}

void update_date(struct tm *tick_time) {
    static char buffer[] = MAX_DATE_STR;  /* FIXME use same buffer, one for both date and time? */

    last_day = tick_time->tm_mday;
    strftime(buffer, sizeof(buffer), DATE_FMT_STR, tick_time);
    text_layer_set_text(date_layer, buffer);
}

void setup_date(Window *window)
{
    /* Create date TextLayer */
    date_layer = text_layer_create(DATE_POS);
    text_layer_set_background_color(date_layer, GColorClear);
    text_layer_set_text_color(date_layer, time_color);
    text_layer_set_text(date_layer, MAX_DATE_STR);

    /* Apply to TextLayer */
#ifdef USE_TIME_FONT_FOR_DATE
    text_layer_set_font(date_layer, time_font);
#else
    text_layer_set_font(date_layer, fonts_get_system_font(FONT_DATE_SYSTEM_NAME));
#endif /*  USE_TIME_FONT_FOR_DATE */
    text_layer_set_text_alignment(date_layer, DATE_ALIGN);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
}

void cleanup_date()
{
    text_layer_destroy(date_layer);
}

/*
** If bounds is GRectZero then use whole watch screen
*/
void setup_bg_image(Window *window, uint32_t resource_id, GRect bounds)
{
    // Create GBitmap, then set to created BitmapLayer
    background_bitmap = gbitmap_create_with_resource(resource_id);

    if (bounds.origin.x == 0 &&
        bounds.origin.y == 0 &&
        bounds.size.w == 0 &&
        bounds.size.h == 0)
    {
        bounds = layer_get_bounds(window_get_root_layer(window));
    }
    background_layer = bitmap_layer_create(bounds);

    bitmap_layer_set_bitmap(background_layer, background_bitmap);

#ifdef PBL_BW
     bitmap_layer_set_compositing_mode(background_layer, GCompOpAssign);
#elif PBL_COLOR
     bitmap_layer_set_compositing_mode(background_layer, GCompOpSet);
#endif

    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(background_layer));
}

void cleanup_bg_image()
{
    /* Destroy GBitmap */
    if (background_bitmap)
    {
        gbitmap_destroy(background_bitmap);
    }

    /* Destroy BitmapLayer */
    if (background_layer)
    {
        bitmap_layer_destroy(background_layer);
    }
}


void update_time() {
    // Get a tm structure
    time_t    temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Create a long-lived buffer
    static char buffer[] = MAX_TIME_STR;

#ifdef DEBUG_TIME
    {
        static int     str_counter=0;

        #ifdef DEBUG_TIME_SCREENSHOT
            strcpy(buffer, debug_time_list[3]);
            light_enable(true);  // mostly for emulator
        #else
            strcpy(buffer, debug_time_list[str_counter]);
            str_counter++;
            if (debug_time_list[str_counter] == NULL)
            {
                str_counter = 0;
            }
        #endif /* DEBUG_TIME_SCREENSHOT */
    }
#else
    {
        // Write the current hours and minutes into the buffer
        if(clock_is_24h_style() == true) {
            // 24h hour format
            strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
        } else {
            // 12 hour format
            strftime(buffer, sizeof(buffer), "%I:%M", tick_time); // produces leading zero for hour and minute
#ifdef REMOVE_LEADING_ZERO_FROM_TIME
            if (buffer[0] == '0')
            {
                memmove(&buffer[0], &buffer[1], sizeof(buffer) - 1); // remove leading zero
            }
#endif /* REMOVE_LEADING_ZERO_FROM_TIME */
        }
    }
#endif /* DEBUG_TIME */

#ifndef NO_DATE
    /* Update the date only when the day changes */
    if (last_day != tick_time->tm_mday)
    {
        update_date(tick_time);
    }
#endif /* NO_DATE */

    // Display this time on the TextLayer
    text_layer_set_text(time_layer, buffer);

#ifdef DEBUG_TIME_PAUSE
    psleep(DEBUG_TIME_PAUSE);
#endif /* DEBUG_TIME_PAUSE */
}

void main_window_load(Window *window) {
    window_set_background_color(window, background_color);

#ifdef BG_IMAGE
    setup_bg_image(window, BG_IMAGE, BG_IMAGE_GRECT);
#endif /* BG_IMAGE */

    // Create time TextLayer
    time_layer = text_layer_create(CLOCK_POS);
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, time_color);
    text_layer_set_text(time_layer, "00:00");

#ifdef FONT_NAME
    // Create GFont
    time_font = fonts_load_custom_font(resource_get_handle(FONT_NAME));
#else
    time_font = fonts_get_system_font(FONT_SYSTEM_NAME);
#endif /* FONT_NAME */

    // Apply to TextLayer
    text_layer_set_font(time_layer, time_font);
    /* Consider GTextAlignmentLeft (with monospaced font) in cases where colon is proportional */
    text_layer_set_text_alignment(time_layer, TIME_ALIGN);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));

#ifndef NO_DATE
    setup_date(window);
#endif /* NO_DATE */
#ifndef NO_BATTERY
    setup_battery(window);
#endif /* NO_BATTERY */
#ifndef NO_BLUETOOTH
    setup_bluetooth(window);
#endif /* NO_BLUETOOTH */

#ifdef USE_SHADOW_TIME_EFFECT
    setup_effects(window);
#endif /* USE_SHADOW_TIME_EFFECT */

    /* Make sure the time is displayed from the start */
    update_time();

#ifndef NO_BATTERY
    /* Ensure battery status is displayed from the start */
    handle_battery(battery_state_service_peek());
#endif /* NO_BATTERY */
}

void main_window_unload(Window *window) {
#ifdef USE_SHADOW_TIME_EFFECT
    cleanup_effects();
#endif /* USE_SHADOW_TIME_EFFECT */

#ifndef NO_BLUETOOTH
    cleanup_bluetooth();
#endif /* NO_BLUETOOTH */
#ifndef NO_BATTERY
    cleanup_battery();
#endif /* NO_BATTERY */
#ifndef NO_DATE
    cleanup_date();
#endif /* NO_DATE */

#ifdef FONT_NAME
    /* Unload GFonts */
    fonts_unload_custom_font(time_font);
#endif /* FONT_NAME */

#ifdef BG_IMAGE
    cleanup_bg_image();
#endif /* BG_IMAGE */

    /* Destroy TextLayers */
    text_layer_destroy(time_layer);


    /* unsubscribe events */
    tick_timer_service_unsubscribe();
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

#ifdef DEBUG_TIME
void debug_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}
#endif /* DEBUG_TIME */

void deinit() {
    // Destroy Window
    window_destroy(main_window);
}

void in_recv_handler(DictionaryIterator *iterator, void *context)
{
    Tuple *t=NULL;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "in_recv_handler() called");
    t = dict_read_first(iterator);
    while(t != NULL)
    {
        switch(t->key)
        {
            case KEY_TIME_COLOR:
                APP_LOG(APP_LOG_LEVEL_DEBUG, "got KEY_TIME_COLOR");
                config_time_color = (int)t->value->int32;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Persisting time color: 0x%06x", config_time_color);
                persist_write_int(KEY_TIME_COLOR, config_time_color);
                time_color = COLOR_FALLBACK(GColorFromHEX(config_time_color), GColorWhite);
                text_layer_set_text_color(time_layer, time_color);
                text_layer_set_text_color(date_layer, time_color);
                text_layer_set_text_color(battery_layer, time_color);
                text_layer_set_text_color(bluetooth_layer, time_color);
                break;

            case KEY_BACKGROUND_COLOR:
                APP_LOG(APP_LOG_LEVEL_DEBUG, "got KEY_BACKGROUND_COLOR");
                config_background_color = (int)t->value->int32;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Persisting background color: 0x%06x", config_background_color);
                persist_write_int(KEY_BACKGROUND_COLOR, config_background_color);
                background_color = COLOR_FALLBACK(GColorFromHEX(config_background_color), GColorWhite); // FIXME Aplite colors inverted?
                window_set_background_color(main_window, background_color);
                break;

            case KEY_VIBRATE_ON_DISCONNECT:
                APP_LOG(APP_LOG_LEVEL_DEBUG, "got KEY_VIBRATE_ON_DISCONNECT");
                config_time_vib_on_disconnect = (bool)t->value->int32;  /* this doesn't feel correct... */
                APP_LOG(APP_LOG_LEVEL_INFO, "Persisting vib_on_disconnect: %d", (int) config_time_vib_on_disconnect);
                persist_write_bool(KEY_VIBRATE_ON_DISCONNECT, config_time_vib_on_disconnect);
                break;

            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Unknown key %d! :-(", (int) t->key);
                break;
        }
        t = dict_read_next(iterator);
    }
}


void init()
{
    time_color = DEFAULT_TIME_COLOR;
    background_color = DEFAULT_BACKGROUND_COLOR;

#ifdef PBL_COLOR
    /* TODO refactor */
    if (persist_exists(KEY_TIME_COLOR))
    {
        config_time_color = persist_read_int(KEY_TIME_COLOR);
        APP_LOG(APP_LOG_LEVEL_INFO, "Read time color: %x", config_time_color);
        time_color = COLOR_FALLBACK(GColorFromHEX(config_time_color), GColorWhite);
    }
    if (persist_exists(KEY_BACKGROUND_COLOR))
    {
        config_background_color = persist_read_int(KEY_BACKGROUND_COLOR);
        APP_LOG(APP_LOG_LEVEL_INFO, "Read background color: %x", config_background_color);
        background_color = COLOR_FALLBACK(GColorFromHEX(config_background_color), GColorBlack);
    }
#endif /* PBL_COLOR */

    if (persist_exists(KEY_VIBRATE_ON_DISCONNECT))
    {
        config_time_vib_on_disconnect = persist_read_bool(KEY_VIBRATE_ON_DISCONNECT);
        APP_LOG(APP_LOG_LEVEL_INFO, "Read vib_on_disconnect: %d", (int) config_time_vib_on_disconnect);
    }

    // Create main Window element and assign to pointer
    main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(main_window, (WindowHandlers) {
                                   .load = MAIN_WINDOW_LOAD,
                                   .unload = MAIN_WINDOW_UNLOAD
                               });

    // Show the Window on the watch, with animated=true
    window_stack_push(main_window, true);

    /* Register events; TickTimerService, Battery */
    tick_timer_service_subscribe(MINUTE_UNIT, TICK_HANDLER);
#ifdef DEBUG_TIME
    #ifndef DEBUG_TIME_SCREENSHOT
        tick_timer_service_subscribe(SECOND_UNIT, DEBUG_TICK_HANDLER);
    #endif /* DEBUG_TIME_SCREENSHOT */
#endif /* DEBUG_TIME */

    /* TODO use AppSync instead? */
    app_message_register_inbox_received(in_recv_handler);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum()); 
}


#ifdef USE_GENERIC_MAIN
int main(void) {
    init();
    app_event_loop();
    deinit();
}
#endif /* USE_GENERIC_MAIN */
