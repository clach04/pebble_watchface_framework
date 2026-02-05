#pragma once

#define USE_GENERIC_MAIN
#undef REMOVE_LEADING_ZERO_FROM_TIME  /* 12 hour display will not have a leading '0' or ' ' if this is defined */
#undef FONT_NAME
#undef FONT_SYSTEM_NAME  /* the default font system will be used */
#undef DEBUG_TIME
//#define USE_TIME_MACHINE  // NOTE mixing with DEBUG_TIME doesn't make sense. Requires update to package.json for dependencies

// Show step count using builtin code
#define USE_HEALTH
//#define UPDATE_HEALTH_ON_ACTIVITY  /* If not set, only updates step count display once per minute */

#define DRAW_BATTERY
#define DRAW_SMALL_BATTERY

//#define QUIET_TIME_IMAGE RESOURCE_ID_IMAGE_QUIET_TIME
//#define QUIET_TIME_IMAGE_GRECT GRect(20, 20, 20, 20)  // Example assumes a 20x20 image

#ifdef PBL_ROUND /* 180x180 */
/*TODO center/move right*/
    #define CLOCK_POS GRect(0, 52, 180, 180) /* probably taller than really needed */
    #define HEALTH_POS GRect(0, 40, 180, 180)
    #define BT_POS GRect(0, 100, 180, 180) /* probably taller than really needed */

    #define DATE_ALIGN GTextAlignmentCenter
    #define DATE_POS GRect(0, 120, 180, 180) /* probably taller than really needed */

    #define BAT_ALIGN GTextAlignmentCenter
    #ifdef DRAW_BATTERY
        #define BAT_POS GRect(85, 10, 180, 180) /* probably taller than really needed */
    #else
        #define BAT_POS GRect(0, 140, 180, 180) /* probably taller than really needed */
    #endif /* DRAW_BATTERY */
#else /* PBL_RECT 144x168 */
    #if PBL_DISPLAY_HEIGHT == 228  // 200x228 Pebble Time 2 (emery)
        // TODO center? shift slight to right a little more, and down
        #define CLOCK_POS GRect(0, 52, 200, 228) /* probably taller than really needed */
        #define HEALTH_POS GRect(0, 40, 200, 228)
        // below untested
        #define BT_POS GRect(0, 120, 200, 228) /* probably taller than really needed */
        // FIXME below should be lower...
        #define DATE_POS GRect(0, 205, 200, 228) /* probably taller than really needed */
        #ifdef DRAW_BATTERY
            #define BAT_POS GRect(5, 215, 200, 228)
        #else
            #define BAT_POS GRect(0, 205, 200, 228) /* probably taller than really needed */
        #endif /* DRAW_BATTERY */
    #else  //  144x168  Original pebbles; Pebble Classic (aplite), Pebble Time (basalt), Pebble 2 (diorite), Pebble 2 Duo (flint)
        #define CLOCK_POS GRect(0, 52, 144, 168) /* probably taller than really needed */
        #define HEALTH_POS GRect(0, 40, 144, 168)
        #define BT_POS GRect(0, 120, 144, 168) /* probably taller than really needed */
        #define DATE_POS GRect(0, 140, 144, 168) /* probably taller than really needed */
        #ifdef DRAW_BATTERY
            #define BAT_POS GRect(5, 150, 144, 168)
        #else
            #define BAT_POS GRect(0, 140, 144, 168) /* probably taller than really needed */
        #endif /* DRAW_BATTERY */
    #endif  // end of original rectangle size
#endif /* end of Round or rectangle */

/* for screen shots and font testing
#define DEBUG_TIME
#define DEBUG_TIME_SCREENSHOT
// ensure quiet time and bluetooth disconnection info is shown
#ifndef quiet_time_is_active  // so not aplite
#define quiet_time_is_active() true  // DEBUG!
#endif
#define bluetooth_connection_service_peek() false  // DEBUG!
*/
