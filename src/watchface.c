#include <pebble.h>
#include "watchface.h"

/* TODO remove leading "s_" prefix */

Window    *s_main_window=NULL;
TextLayer *s_time_layer=NULL;
TextLayer *s_date_layer=NULL;
TextLayer *s_battery_layer=NULL;
TextLayer *s_bluetooth_layer=NULL;

GFont       s_time_font;
#ifdef BG_IMAGE
BitmapLayer *s_background_layer=NULL;
GBitmap     *s_background_bitmap=NULL;
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
    "11:11",
    "12:34",
    "22:22",
    "23:50",
    "25:55",
    /* 12 hour format withOUT leading zero, or space */
    "1:01",
    "8:55",
    /* 12 hour format withOUT leading zero, with  space */
    " 1:01",
    " 8:55",
    NULL
};
#endif /* DEBUG_TIME */

void handle_bluetooth(bool connected)
{
    /* TODO use gfx not text */
    if (connected)
    {
        text_layer_set_text(s_bluetooth_layer, "");
    }
    else
    {
        text_layer_set_text(s_bluetooth_layer, BLUETOOTH_DISCONNECTED_STR);
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
    s_bluetooth_layer = text_layer_create(BT_POS);
    text_layer_set_text_color(s_bluetooth_layer, time_color);
    text_layer_set_background_color(s_bluetooth_layer, GColorClear);
    text_layer_set_font(s_bluetooth_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(s_bluetooth_layer, BT_ALIGN);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bluetooth_layer));
    text_layer_set_text(s_bluetooth_layer, "");

    handle_bluetooth(bluetooth_connection_service_peek());
    bluetooth_connection_service_subscribe(handle_bluetooth);
}

void cleanup_bluetooth()
{
    bluetooth_connection_service_unsubscribe();
    text_layer_destroy(s_bluetooth_layer);
}

void handle_battery(BatteryChargeState charge_state) {
    static char battery_text[] = MAX_BAT_STR;

    if (charge_state.is_charging) {
        snprintf(battery_text, sizeof(battery_text), "Charging");
        text_layer_set_text_color(s_battery_layer, COLOR_FALLBACK(GColorGreen, time_color));
    } else {
        snprintf(battery_text, sizeof(battery_text), BAT_FMT_STR, charge_state.charge_percent);
#ifdef PBL_PLATFORM_BASALT
        /* TODO Check charge level and change color? E.g. red at 10%/20% */
        if (charge_state.charge_percent <= 20)
        {
            text_layer_set_text_color(s_battery_layer, GColorRed);
        }
        else /* TODO different colors for different ranges */
        {
            /* TODO is this an expensive call ? */
            text_layer_set_text_color(s_battery_layer, time_color);
        }
#endif
    }
    text_layer_set_text(s_battery_layer, battery_text);
}

/* Battery level */
void setup_battery(Window *window)
{
    s_battery_layer = text_layer_create(BAT_POS);
    text_layer_set_text_color(s_battery_layer, time_color);
    text_layer_set_background_color(s_battery_layer, GColorClear);
    text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(s_battery_layer, BAT_ALIGN);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
    text_layer_set_text(s_battery_layer, MAX_BAT_STR);

    battery_state_service_subscribe(handle_battery);
}

void cleanup_battery()
{
    battery_state_service_unsubscribe();
    text_layer_destroy(s_battery_layer);
}

void update_date(struct tm *tick_time) {
    static char buffer[] = MAX_DATE_STR;  /* FIXME use same buffer, one for both date and time? */

    last_day = tick_time->tm_mday;
    strftime(buffer, sizeof(buffer), DATE_FMT_STR, tick_time);
    text_layer_set_text(s_date_layer, buffer);
}

void setup_date(Window *window)
{
    /* Create date TextLayer */
    s_date_layer = text_layer_create(DATE_POS);
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, time_color);
    text_layer_set_text(s_date_layer, MAX_DATE_STR);

    /* Apply to TextLayer */
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

void cleanup_date()
{
    text_layer_destroy(s_date_layer);
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

        strcpy(buffer, debug_time_list[str_counter]);
        str_counter++;
        if (debug_time_list[str_counter] == NULL)
        {
            str_counter = 0;
        }
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

    /* Update the date only when the day changes */
    if (last_day != tick_time->tm_mday)
    {
        update_date(tick_time);
    }

    // Display this time on the TextLayer
    text_layer_set_text(s_time_layer, buffer);
}

void main_window_load(Window *window) {
#ifdef BG_IMAGE
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Create GBitmap, then set to created BitmapLayer
    s_background_bitmap = gbitmap_create_with_resource(BG_IMAGE);
    
    s_background_layer = bitmap_layer_create(bounds);
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);

#ifdef PBL_PLATFORM_APLITE
     bitmap_layer_set_compositing_mode(s_background_layer, GCompOpAssign);
#elif PBL_PLATFORM_BASALT
     bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
#endif
#endif /* BG_IMAGE */

    window_set_background_color(window, background_color);

#ifdef BG_IMAGE
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
#endif /* BG_IMAGE */

    // Create time TextLayer
    s_time_layer = text_layer_create(CLOCK_POS);
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, time_color);
    text_layer_set_text(s_time_layer, "00:00");

#ifdef FONT_NAME
    // Create GFont
    s_time_font = fonts_load_custom_font(resource_get_handle(FONT_NAME));
#else
    s_time_font = fonts_get_system_font(FONT_SYSTEM_NAME);
#endif /* FONT_NAME */

    // Apply to TextLayer
    text_layer_set_font(s_time_layer, s_time_font);
    /* Consider GTextAlignmentLeft (with monospaced font) in cases where colon is proportional */
    text_layer_set_text_alignment(s_time_layer, TIME_ALIGN);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

    setup_date(window);
    setup_battery(window);
    setup_bluetooth(window);

    /* Make sure the time is displayed from the start */
    update_time();
    /* Ensure battery status is displayed from the start */
    handle_battery(battery_state_service_peek());
}

void main_window_unload(Window *window) {
    cleanup_bluetooth();
    cleanup_battery();
    cleanup_date();

#ifdef FONT_NAME
    /* Unload GFonts */
    fonts_unload_custom_font(s_time_font);
#endif /* FONT_NAME */

#ifdef BG_IMAGE
    /* Destroy GBitmap */
    gbitmap_destroy(s_background_bitmap);

    /* Destroy BitmapLayer */
    bitmap_layer_destroy(s_background_layer);
#endif /* BG_IMAGE */

    /* Destroy TextLayers */
    text_layer_destroy(s_time_layer);


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
    window_destroy(s_main_window);
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
                text_layer_set_text_color(s_time_layer, time_color);
                text_layer_set_text_color(s_date_layer, time_color);
                text_layer_set_text_color(s_bluetooth_layer, time_color);
                break;

            case KEY_BACKGROUND_COLOR:
                APP_LOG(APP_LOG_LEVEL_DEBUG, "got KEY_BACKGROUND_COLOR");
                config_background_color = (int)t->value->int32;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Persisting background color: 0x%06x", config_background_color);
                persist_write_int(KEY_BACKGROUND_COLOR, config_background_color);
                background_color = COLOR_FALLBACK(GColorFromHEX(config_background_color), GColorWhite); // FIXME Aplite colors inverted?
                window_set_background_color(s_main_window, background_color);
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

#ifdef PBL_PLATFORM_BASALT
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
#endif /* PBL_PLATFORM_BASALT */

    if (persist_exists(KEY_VIBRATE_ON_DISCONNECT))
    {
        config_time_vib_on_disconnect = persist_read_bool(KEY_VIBRATE_ON_DISCONNECT);
        APP_LOG(APP_LOG_LEVEL_INFO, "Read vib_on_disconnect: %d", (int) config_time_vib_on_disconnect);
    }

    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
                                   .load = MAIN_WINDOW_LOAD,
                                   .unload = MAIN_WINDOW_UNLOAD
                               });

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    /* Register events; TickTimerService, Battery */
    tick_timer_service_subscribe(MINUTE_UNIT, TICK_HANDLER);
#ifdef DEBUG_TIME
    tick_timer_service_subscribe(SECOND_UNIT, debug_tick_handler);
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
