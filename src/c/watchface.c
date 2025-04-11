#include <pebble.h>
#include <pebble_process_info.h>  // ONLY for get_major_app_version()
extern const PebbleProcessInfo __pbl_app_info;  // ONLY for get_major_app_version()
    
#ifdef USE_SHADOW_TIME_EFFECT
#include "effect_layer.h"  /* from https://github.com/ygalanter/EffectLayer */
#endif /* USE_SHADOW_TIME_EFFECT */

#include "watchface.h"


#ifdef PBL_BW
    #ifndef GColorFromHEX
        /* Pre SDk 3.8 */
        #define GColorFromHEX(int_color) int_color == 0 ? GColorBlack : GColorWhite
    #endif /* GColorFromHEX */
#endif /* PBL_BW */

Window    *main_window=NULL;
#ifndef NO_TEXT_TIME_LAYER
TextLayer *time_layer=NULL;
#endif /* NO_TEXT_TIME_LAYER */

TextLayer *date_layer=NULL;
#ifndef DRAW_BATTERY
TextLayer *battery_layer=NULL;
#else
Layer *battery_layer=NULL;
    #ifdef DRAW_SMALL_BATTERY
        const GPathInfo battery_path_info={ .num_points = 14, .points = ((GPoint[]){{0,6},{0,0},{10,0},{10,2},{11,2},{11,4},{10,4},{10,6},{0,6},{1,5},{9,5},{9,1},{1,1},{1,5}}) };
    #else
        const GPathInfo battery_path_info={ .num_points = 15, .points = ((GPoint[]){{0,10},{0,0},{20,0},{20,4},{22,4},{22,8},{20,8},{20,12},{0,12},{0,10},{2,10},{18,10},{18,2},{2,2},{2,10}}) };
    #endif /* DRAW_SMALL_BATTERY */
GPath *battery_path=NULL;
#endif /* DRAW_BATTERY */
GColor     battery_color;
TextLayer *bluetooth_tlayer=NULL;
TextLayer *health_tlayer=NULL;

GFont       time_font;
#ifdef BG_IMAGE
BitmapLayer *background_layer=NULL;
GBitmap     *background_bitmap=NULL;
#endif /* BG_IMAGE */
#ifdef BT_DISCONNECT_IMAGE
BitmapLayer *bluetooth_blayer=NULL;
GBitmap     *bluetooth_disconnect_bitmap=NULL;
#endif /* BT_DISCONNECT_IMAGE */
/* For colors, see http://developer.getpebble.com/tools/color-picker/#0000FF */
GColor       time_color;  /* NOTE used for date too */
GColor       background_color;
int          config_time_color;
int          config_background_color;
bool         config_time_vib_on_disconnect = false;
static int major_version = 0;


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

/* TODO resequence? */
void setup_bt_image(Window *window, uint32_t resource_id, GRect bounds);
void cleanup_bt_image();


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
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() bluetooth status %d", __func__, (int) connected);
    /* TODO use gfx not text */
    if (connected)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() bluetooth connected", __func__);
        #ifdef BT_DISCONNECT_IMAGE
            bitmap_layer_set_bitmap(bluetooth_blayer, NULL); /* Show nothing */
            /*
            ** alternative is layer_set_hidden()
            **  https://developer.getpebble.com/docs/c/User_Interface/Layers/#layer_set_hidden
            */
        #else /* BT_DISCONNECT_IMAGE */
            text_layer_set_text(bluetooth_tlayer, "");
        #endif /* BT_DISCONNECT_IMAGE */
    }
    else
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() bluetooth DISconnected", __func__); // TODO open issue with Pebble - Why is this NOT showing up in debug log? Presumbly as BT is needed even in emulator and logging is thrown away without connection?
        #ifdef BT_DISCONNECT_IMAGE
            bitmap_layer_set_bitmap(bluetooth_blayer, bluetooth_disconnect_bitmap);
        #else /* BT_DISCONNECT_IMAGE */
            text_layer_set_text(bluetooth_tlayer, BLUETOOTH_DISCONNECTED_STR);
        #endif /* BT_DISCONNECT_IMAGE */
        if (config_time_vib_on_disconnect && (bluetooth_state != connected))
        {
            bool do_vib = true;

            // check if quiet time
            if (quiet_time_is_active())
            {
                do_vib = false;
            }
#ifdef USE_HEALTH
            // check if asleep
            if (do_vib)
            {
                HealthActivityMask activities = health_service_peek_current_activities();

                if (activities & HealthActivitySleep)
                {
                    do_vib = false;
                }
                if (activities & HealthActivityRestfulSleep)
                {
                    do_vib = false;
                }
            }
#endif // USE_HEALTH

            /* had BT connection then lost it, rather than started disconnected */
            if (do_vib)
            {
                vibes_short_pulse();  /* vibrate/rumble */
            }
        }
    }
    bluetooth_state = connected;
}


void setup_bluetooth(Window *window)
{
#ifdef BT_DISCONNECT_IMAGE
    #ifdef BT_DISCONNECT_IMAGE_GRECT
        setup_bt_image(window, BT_DISCONNECT_IMAGE, BT_DISCONNECT_IMAGE_GRECT);
    #else /* BT_DISCONNECT_IMAGE_GRECT */
        setup_bt_image(window, BT_DISCONNECT_IMAGE, GRectZero);
    #endif /* BT_DISCONNECT_IMAGE_GRECT */
#else
    /* text */
    bluetooth_tlayer = text_layer_create(BT_POS);
    text_layer_set_text_color(bluetooth_tlayer, time_color);
    text_layer_set_background_color(bluetooth_tlayer, GColorClear);
    text_layer_set_font(bluetooth_tlayer, fonts_get_system_font(FONT_BT_SYSTEM_NAME));
    text_layer_set_text_alignment(bluetooth_tlayer, BT_ALIGN);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(bluetooth_tlayer));
    text_layer_set_text(bluetooth_tlayer, "");
#endif /* BT_DISCONNECT_IMAGE */

    handle_bluetooth(bluetooth_connection_service_peek());
    bluetooth_connection_service_subscribe(handle_bluetooth);
}

void cleanup_bluetooth()
{
    bluetooth_connection_service_unsubscribe();
#ifdef BT_DISCONNECT_IMAGE
    cleanup_bt_image();
#else
    text_layer_destroy(bluetooth_tlayer);
#endif /* BT_DISCONNECT_IMAGE */
}

/*****************/
#if defined(PBL_HEALTH)
static void health_handler(HealthEventType event, void *context)
{
    // Which type of event occured?
    switch(event)
    {
        case HealthEventSignificantUpdate:
            APP_LOG(APP_LOG_LEVEL_INFO, "New HealthService HealthEventSignificantUpdate event");
            //break;
        case HealthEventMovementUpdate:
            APP_LOG(APP_LOG_LEVEL_INFO, "New HealthService HealthEventMovementUpdate event");
            update_health();
            break;
        case HealthEventSleepUpdate:
            APP_LOG(APP_LOG_LEVEL_INFO, "New HealthService HealthEventSleepUpdate event");
            break;
#if PBL_API_EXISTS(health_service_register_metric_alert)
        case HealthEventMetricAlert:
            APP_LOG(APP_LOG_LEVEL_INFO, "New HealthService HealthEventMetricAlert event");
            break;
#endif  // HealthEventMetricAlert
#if PBL_API_EXISTS(health_service_set_heart_rate_sample_period)
        case HealthEventHeartRateUpdate:
            APP_LOG(APP_LOG_LEVEL_INFO, "New HealthService HealthEventHeartRateUpdate event");
            break;
#endif // HealthEventHeartRateUpdate
        // Default is not a good idea, but get switch warnings (about heart events) without it on Basalt/Chalk SDK4 builds
        default:
            APP_LOG(APP_LOG_LEVEL_INFO, "New HealthService UNHANDLED event");
            break;
    }
}

void setup_health(Window *window)
{
    health_tlayer = text_layer_create(HEALTH_POS);
    text_layer_set_text_color(health_tlayer, time_color);
    text_layer_set_background_color(health_tlayer, GColorClear);
    text_layer_set_font(health_tlayer, fonts_get_system_font(FONT_DATE_SYSTEM_NAME));
    text_layer_set_text_alignment(health_tlayer, HEALTH_ALIGN);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(health_tlayer));
    text_layer_set_text(health_tlayer, "");

#ifdef UPDATE_HEALTH_ON_ACTIVITY
    if(!health_service_events_subscribe(health_handler, NULL))
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    }
#endif /* UPDATE_HEALTH_ON_ACTIVITY */
}

void update_health()
{
    /* Create a long-lived buffer */
    static char buffer[] = MAX_HEALTH_STR;

    time_t start = time_start_of_today();
    time_t end = time(NULL);  /* Now */

    /* Check data is available */
    HealthServiceAccessibilityMask result = health_service_metric_accessible(HealthMetricStepCount, start, end);
    if(result & HealthServiceAccessibilityMaskAvailable)
    {
        HealthValue steps = health_service_sum(HealthMetricStepCount, start, end);

        APP_LOG(APP_LOG_LEVEL_INFO, "Steps today: %d", (int)steps);
        snprintf(buffer, sizeof(buffer), HEALTH_FMT_STR, (int) steps);
    }
    else
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "No data available!");
        strcpy(buffer, "");
    }
    text_layer_set_text(health_tlayer, buffer);
}

void cleanup_health()
{
    health_service_events_unsubscribe();
    text_layer_destroy(health_tlayer);
}
#endif /* PBL_HEALTH */
/*****************/

void handle_battery(BatteryChargeState charge_state) {
    battery_color=time_color;
#ifndef DRAW_BATTERY
    static char battery_text[] = MAX_BAT_STR;
#endif /* DRAW_BATTERY */

#ifdef PBL_COLOR
    if (charge_state.is_charging)
    {
        battery_color = COLOR_FALLBACK(GColorGreen, time_color);
    }
    else
    {
        if (charge_state.charge_percent <= 20)
        {
            battery_color = GColorRed;
        }
    }
#endif /* PBL_COLOR */

#ifdef DRAW_BATTERY
    layer_mark_dirty(battery_layer);
#else
    if (charge_state.is_charging)
    {
        snprintf(battery_text, sizeof(battery_text), "Charging");
    }
    else
    {
        snprintf(battery_text, sizeof(battery_text), BAT_FMT_STR, charge_state.charge_percent);
    }
    text_layer_set_text_color(battery_layer, battery_color);
    text_layer_set_text(battery_layer, battery_text);
#endif /* DRAW_BATTERY */
}

#ifdef DRAW_BATTERY
void update_battery_proc(Layer *layer, GContext *ctx)
{
    int pos_x=BAT_POS.origin.x;
    int pos_y=BAT_POS.origin.y;
    BatteryChargeState state=battery_state_service_peek();

    graphics_context_set_fill_color(ctx, battery_color);
    graphics_context_set_stroke_color(ctx, battery_color);

    gpath_move_to(battery_path, GPoint(pos_x, pos_y));
    gpath_draw_outline(ctx, battery_path);
    gpath_draw_filled(ctx, battery_path);
#ifdef DRAW_SMALL_BATTERY
    graphics_fill_rect(ctx, GRect(pos_x + 2, pos_y + 2, state.charge_percent * 7 / 100, 3), 0, 0);
#else
    graphics_fill_rect(ctx, GRect(pos_x + 3, pos_y + 3, state.charge_percent * 15 / 100, 7), 0, 0);
#endif /* DRAW_SMALL_BATTERY */
}
#endif /* DRAW_BATTERY */

/* Battery level */
void setup_battery(Window *window)
{
#ifdef DRAW_BATTERY
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    battery_layer = layer_create(bounds);  // TODO BAT_POS - instead
    battery_path = gpath_create(&battery_path_info);
    layer_set_update_proc(battery_layer, update_battery_proc);
    layer_add_child(window_layer, battery_layer); //??
#else
    battery_layer = text_layer_create(BAT_POS);
    text_layer_set_text_color(battery_layer, time_color);
    text_layer_set_background_color(battery_layer, GColorClear);
    text_layer_set_font(battery_layer, fonts_get_system_font(FONT_BAT_SYSTEM_NAME));
    text_layer_set_text_alignment(battery_layer, BAT_ALIGN);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(battery_layer));
    text_layer_set_text(battery_layer, MAX_BAT_STR);
#endif /* DRAW_BATTERY */

    battery_state_service_subscribe(handle_battery);
}

void cleanup_battery()
{
    battery_state_service_unsubscribe();
#ifdef DRAW_BATTERY
    layer_destroy(battery_layer);
    gpath_destroy(battery_path);
#else
    text_layer_destroy(battery_layer);
#endif /* DRAW_BATTERY */
}

#ifdef QUIET_TIME_IMAGE
BitmapLayer *quiet_time_blayer=NULL;
GBitmap     *quiet_time_bitmap=NULL;

void handle_quiet_time(void)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() entry", __func__);
    if (quiet_time_is_active())
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() quiet_time_is_active", __func__);
        bitmap_layer_set_bitmap(quiet_time_blayer, quiet_time_bitmap);
    }
    else
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() quiet_time_is_not_active", __func__);
        bitmap_layer_set_bitmap(quiet_time_blayer, NULL);
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() exit", __func__);
}

void setup_quiet_time(Window *window)
{
    GRect bounds;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() entry", __func__);
    quiet_time_bitmap = gbitmap_create_with_resource(QUIET_TIME_IMAGE);

    #ifdef QUIET_TIME_IMAGE_GRECT
        bounds = QUIET_TIME_IMAGE_GRECT;
    #else // QUIET_TIME_IMAGE_GRECT
        // use whole watch screen, auto centered
        bounds = layer_get_bounds(window_get_root_layer(window));
    #endif // QUIET_TIME_IMAGE_GRECT

    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() bounds x=%d, y=%d, w=%d, h=%d", __func__, bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);
    quiet_time_blayer = bitmap_layer_create(bounds);

    /* Do not attached image to layer (yet...) */
    bitmap_layer_set_bitmap(quiet_time_blayer, NULL);

    bitmap_layer_set_compositing_mode(quiet_time_blayer, GCompOpSet);
    layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(quiet_time_blayer));

    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() exit", __func__);
}

void cleanup_quiet_time(void)
{
    /* Destroy GBitmap */
    if (quiet_time_bitmap)
    {
        gbitmap_destroy(quiet_time_bitmap);
    }

    /* Destroy BitmapLayer */
    if (quiet_time_blayer)
    {
        bitmap_layer_destroy(quiet_time_blayer);
    }
}
#endif // QUIET_TIME_IMAGE


#ifndef NO_TEXT_TIME_LAYER
void setup_text_time(Window *window)
{
    // Create time TextLayer
    time_layer = text_layer_create(CLOCK_POS);
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, time_color);
    text_layer_set_text(time_layer, "00:00");

    // Apply to TextLayer
    text_layer_set_font(time_layer, time_font);
    /* Consider GTextAlignmentLeft (with monospaced font) in cases where colon is proportional */
    text_layer_set_text_alignment(time_layer, TIME_ALIGN);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}

void cleanup_text_time()
{
    /* Destroy TextLayers */
    text_layer_destroy(time_layer);
}
#endif /* NO_TEXT_TIME_LAYER */

void update_date(struct tm *tick_time) {
    static char buffer[] = MAX_DATE_STR;  /* TODO use same buffer, one for both date and time? */

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

    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() entry", __func__);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() bounds x=%d, y=%d, w=%d, h=%d", __func__, bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);
    if (bounds.origin.x == 0 &&
        bounds.origin.y == 0 &&
        bounds.size.w == 0 &&
        bounds.size.h == 0)
    {
        bounds = layer_get_bounds(window_get_root_layer(window));
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() bounds x=%d, y=%d, w=%d, h=%d", __func__, bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);
    background_layer = bitmap_layer_create(bounds);

    bitmap_layer_set_bitmap(background_layer, background_bitmap);

    bitmap_layer_set_compositing_mode(background_layer, GCompOpSet);

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

#ifdef BT_DISCONNECT_IMAGE
/*
** If bounds is GRectZero then use whole watch screen, auto centered
*/
void setup_bt_image(Window *window, uint32_t resource_id, GRect bounds)
{
    // Create GBitmap, then set to created BitmapLayer
    bluetooth_disconnect_bitmap = gbitmap_create_with_resource(resource_id);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() entry", __func__);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() bounds x=%d, y=%d, w=%d, h=%d", __func__, bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);
    if (bounds.origin.x == 0 &&
        bounds.origin.y == 0 &&
        bounds.size.w == 0 &&
        bounds.size.h == 0)
    {
        bounds = layer_get_bounds(window_get_root_layer(window));
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s() bounds x=%d, y=%d, w=%d, h=%d", __func__, bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);
    bluetooth_blayer = bitmap_layer_create(bounds);

    /* Do not attached image to layer (yet...) */
    bitmap_layer_set_bitmap(bluetooth_blayer, NULL);

    bitmap_layer_set_compositing_mode(bluetooth_blayer, GCompOpSet);
    layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(bluetooth_blayer));
}

void cleanup_bt_image()
{
    /* Destroy GBitmap */
    if (bluetooth_disconnect_bitmap)
    {
        gbitmap_destroy(bluetooth_disconnect_bitmap);
    }

    /* Destroy BitmapLayer */
    if (bluetooth_blayer)
    {
        bitmap_layer_destroy(bluetooth_blayer);
    }
}
#endif /* BT_DISCONNECT_IMAGE_GRECT */


#ifndef NO_TEXT_TIME_LAYER
void update_time(struct tm *tick_time) {
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
        /*
        ** TODO experiment with clock_copy_time_string()
        ** https://developer.getpebble.com/docs/c/Foundation/Wall_Time/
        */
        // Write the current hours and minutes into the buffer
        if(clock_is_24h_style() == true) {
            // 24h hour format
            strftime(buffer, sizeof(buffer), TIME_FMT_STR_24H, tick_time);
        } else {
            // 12 hour format
            strftime(buffer, sizeof(buffer), TIME_FMT_STR_12H, tick_time);
        }
    }
#endif /* DEBUG_TIME */

#ifdef REMOVE_LEADING_ZERO_FROM_TIME
    if(clock_is_24h_style() == false)
    {
        if (buffer[0] == '0' || buffer[0] == ' ')
        {
            memmove(&buffer[0], &buffer[1], sizeof(buffer) - 1); // remove leading character (really byte)
        }
    }
#endif /* REMOVE_LEADING_ZERO_FROM_TIME */

#ifndef NO_DATE
    /* Update the date only when the day changes */
    if (last_day != tick_time->tm_mday)
    {
        update_date(tick_time);
    }
#endif /* NO_DATE */

    // Display this time on the TextLayer
    text_layer_set_text(time_layer, buffer);

#if defined(USE_HEALTH)
    update_health();
#endif /* USE_HEALTH */

#ifdef QUIET_TIME_IMAGE
    handle_quiet_time();
#endif // QUIET_TIME_IMAGE

#ifdef DEBUG_TIME_PAUSE
    psleep(DEBUG_TIME_PAUSE);
#endif /* DEBUG_TIME_PAUSE */
}
#endif /* NO_TEXT_TIME_LAYER */

void main_window_load(Window *window) {
    window_set_background_color(window, background_color);

#ifdef BG_IMAGE
    #ifdef BG_IMAGE_GRECT
        setup_bg_image(window, BG_IMAGE, BG_IMAGE_GRECT);
    #else /* BG_IMAGE_GRECT */
        setup_bg_image(window, BG_IMAGE, GRectZero);
    #endif /* BG_IMAGE_GRECT */
#endif /* BG_IMAGE */

#ifdef FONT_NAME
    // Create GFont
    time_font = fonts_load_custom_font(resource_get_handle(FONT_NAME));
#else
    time_font = fonts_get_system_font(FONT_SYSTEM_NAME);
#endif /* FONT_NAME */

    SETUP_TIME(window);

#ifndef NO_DATE
    setup_date(window);
#endif /* NO_DATE */
#ifndef NO_BATTERY
    setup_battery(window);
#endif /* NO_BATTERY */

#ifndef NO_BLUETOOTH
    /* setup BT layer AFTER (i.e. on top of) time layer */
    setup_bluetooth(window);
#endif /* NO_BLUETOOTH */

#ifdef USE_SHADOW_TIME_EFFECT
    setup_effects(window);
#endif /* USE_SHADOW_TIME_EFFECT */

#ifdef USE_HEALTH
    setup_health(window);
#endif /* USE_HEALTH */

#ifdef QUIET_TIME_IMAGE
    setup_quiet_time(window);
#endif // QUIET_TIME_IMAGE

    /* Make sure the time is displayed from the start */
    // Get a tm structure
    time_t    temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    update_time(tick_time);

#ifndef NO_BATTERY
    /* Ensure battery status is displayed from the start */
    handle_battery(battery_state_service_peek());
#endif /* NO_BATTERY */
}

void main_window_unload(Window *window) {

#ifdef QUIET_TIME_IMAGE
    cleanup_quiet_time();
#endif // QUIET_TIME_IMAGE

#ifdef USE_HEALTH
    cleanup_health();
#endif /* USE_HEALTH */

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

    CLEANUP_TIME();

    /* unsubscribe events */
#ifdef TIME_MACHINE
    time_machine_tick_timer_service_unsubscribe();
#else
    tick_timer_service_unsubscribe();
#endif
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time(tick_time);
}

#ifdef DEBUG_TIME
void debug_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time(tick_time);
}
#endif /* DEBUG_TIME */

void deinit() {
    // Destroy Window
    window_destroy(main_window);
}

void in_recv_handler(DictionaryIterator *iterator, void *context)
{
    Tuple *t=NULL;
    bool wrote_config=false;
    bool custom_wrote_config=false;

    /* NOTE if new entries are added, increase MAX_MESSAGE_SIZE_OUT macro */

    APP_LOG(APP_LOG_LEVEL_DEBUG, "in_recv_handler() called");
    t = dict_find(iterator, MESSAGE_KEY_BACKGROUND_COLOR);
    if (t)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_BACKGROUND_COLOR");
        if (config_background_color != (int)t->value->int32)  // only update config if setting is different
        {
            config_background_color = (int)t->value->int32;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Persisting background color: 0x%06x", config_background_color);
            persist_write_int(MESSAGE_KEY_BACKGROUND_COLOR, config_background_color);
            wrote_config = true;
            background_color = GColorFromHEX(config_background_color);
            window_set_background_color(main_window, background_color);
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "BACKGROUND COLOR DONE");
    }

    t = dict_find(iterator, MESSAGE_KEY_VIBRATE_ON_DISCONNECT);
    if (t)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_VIBRATE_ON_DISCONNECT");
        if (config_time_vib_on_disconnect != (bool)t->value->int32)  /* this doesn't feel correct... */
        {
            config_time_vib_on_disconnect = (bool)t->value->int32;  /* this doesn't feel correct... */
            APP_LOG(APP_LOG_LEVEL_INFO, "Persisting vib_on_disconnect: %d", (int) config_time_vib_on_disconnect);
            persist_write_bool(MESSAGE_KEY_VIBRATE_ON_DISCONNECT, config_time_vib_on_disconnect);
            wrote_config = true;
        }
    }

    t = dict_find(iterator, MESSAGE_KEY_TIME_COLOR);
    if (t)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_TIME_COLOR");
        if (config_time_color != (int)t->value->int32)
        {
            config_time_color = (int)t->value->int32;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Persisting time color: 0x%06x", config_time_color);
            persist_write_int(MESSAGE_KEY_TIME_COLOR, config_time_color);
            wrote_config = true;
        }
        time_color = GColorFromHEX(config_time_color);
#ifndef NO_TEXT_TIME_LAYER
        text_layer_set_text_color(time_layer, time_color);
#endif /* NO_TEXT_TIME_LAYER */

        if (date_layer) /* or #ifndef NO_DATE */
        {
            text_layer_set_text_color(date_layer, time_color);
        }
        if (bluetooth_tlayer)
        {
            text_layer_set_text_color(bluetooth_tlayer, time_color);
        }
        if (health_tlayer)
        {
            text_layer_set_text_color(health_tlayer, time_color);
        }
        // battery done last, in case battery draw (i.e. NOT text) is used
        // and mark dirty is called (forgets to then repaint other stuff)
        if (battery_layer)
        {
            #ifndef NO_BATTERY
            handle_battery(battery_state_service_peek());
            #endif /* NO_BATTERY */
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "TIME COLOR DONE");
    }
    /* NOTE if new entries are added, increase MAX_MESSAGE_SIZE_OUT macro */

#ifdef CUSTOM_IN_RECV_HANDLER
    custom_wrote_config = CUSTOM_IN_RECV_HANDLER(iterator, context);
#endif /* CUSTOM_IN_RECV_HANDLER */

    if (wrote_config || custom_wrote_config)
    {
        int stored_major_version = -1;
        if (persist_exists(MESSAGE_KEY_MAJOR_VERSION))
        {
            stored_major_version = persist_read_int(MESSAGE_KEY_MAJOR_VERSION);
        }
        if (stored_major_version != major_version)
            persist_write_int(MESSAGE_KEY_MAJOR_VERSION, major_version);
    }
}

void wipe_config()
{
    (void) persist_delete(MESSAGE_KEY_TIME_COLOR);
    (void) persist_delete(MESSAGE_KEY_BACKGROUND_COLOR);
    (void) persist_delete(MESSAGE_KEY_VIBRATE_ON_DISCONNECT);
}

int get_major_app_version()
{
    /*
    ** Internal API subject to change!
    ** https://forums.pebble.com/t/how-can-i-get-my-app-version-in-c-code/7959
    */
    return __pbl_app_info.process_version.major;
}

void init()
{
    time_color = DEFAULT_TIME_COLOR;
    background_color = DEFAULT_BACKGROUND_COLOR;

    major_version = get_major_app_version();
    APP_LOG(APP_LOG_LEVEL_INFO, "get_major_app_version: %d", major_version);
    
    if (persist_exists(MESSAGE_KEY_MAJOR_VERSION))
    {
        int stored_major_version = persist_read_int(MESSAGE_KEY_MAJOR_VERSION);

        if(stored_major_version > major_version)
        {
            /* Upgrade logic goes here */
            wipe_config();  // Quick and dirty
        }
        /*
        ** Minor version bumps are assumed to be settings compatible
        ** (override this logic if that is not true).
        */
    }
    else
    {
        /* Does NOT exist - wipe out settings just in case... */
        wipe_config();
    }

    /* TODO refactor */
    if (persist_exists(MESSAGE_KEY_TIME_COLOR))
    {
        config_time_color = persist_read_int(MESSAGE_KEY_TIME_COLOR);
        APP_LOG(APP_LOG_LEVEL_INFO, "Read time color: %x", config_time_color);
        time_color = GColorFromHEX(config_time_color);
    }
    if (persist_exists(MESSAGE_KEY_BACKGROUND_COLOR))
    {
        config_background_color = persist_read_int(MESSAGE_KEY_BACKGROUND_COLOR);
        APP_LOG(APP_LOG_LEVEL_INFO, "Read background color: %x", config_background_color);
        background_color = GColorFromHEX(config_background_color);
    }

    if (persist_exists(MESSAGE_KEY_VIBRATE_ON_DISCONNECT))
    {
        config_time_vib_on_disconnect = persist_read_bool(MESSAGE_KEY_VIBRATE_ON_DISCONNECT);
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
#ifdef TIME_MACHINE
    time_t now = time(NULL);
    struct tm* time_machine_start = localtime(&now);

    // Override start time to midnight, (local time)
    time_machine_start->tm_hour = time_machine_start->tm_min = 0;

    time_machine_init(time_machine_start, TIME_MACHINE_MINUTES, 1000);  // accelerate minutes to seconds
    //time_machine_init(time_machine_start, TIME_MACHINE_HOURS, 1000);  // accelerate hours to seconds
    time_machine_tick_timer_service_subscribe(TICK_HANDLER_INTERVAL, TICK_HANDLER);
#else
    tick_timer_service_subscribe(TICK_HANDLER_INTERVAL, TICK_HANDLER);
#endif
#ifdef DEBUG_TIME
    #ifndef DEBUG_TIME_SCREENSHOT
        tick_timer_service_subscribe(SECOND_UNIT, DEBUG_TICK_HANDLER);
    #endif /* DEBUG_TIME_SCREENSHOT */
#endif /* DEBUG_TIME */

    /* TODO use AppSync instead? */
    app_message_register_inbox_received(in_recv_handler);
#ifdef MAX_MESSAGE_SIZES
    app_message_open(MAX_MESSAGE_SIZE_IN, MAX_MESSAGE_SIZE_OUT);
#else /* MAX_MESSAGE_INBOX_SIZE */
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
#endif /* MAX_MESSAGE_INBOX_SIZE */
}


#ifdef USE_GENERIC_MAIN
int main(void) {
    init();
    app_event_loop();
    deinit();
}
#endif /* USE_GENERIC_MAIN */
