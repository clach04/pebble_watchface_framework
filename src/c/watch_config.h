#pragma once

#define USE_GENERIC_MAIN
#undef REMOVE_LEADING_ZERO_FROM_TIME  /* 12 hour display will not have a leading '0' or ' ' if this is defined */
#undef FONT_NAME
#undef FONT_SYSTEM_NAME  /* the default font system will be used */
// See https://developer.rebble.io/guides/app-resources/system-fonts/ for builtin system fonts
// See https://github.com/clach04/pebble-app-font-browser/releases to see fonts on device
#undef DEBUG_TIME
//#define USE_TIME_MACHINE  // NOTE mixing with DEBUG_TIME doesn't make sense. Requires update to package.json for dependencies

// Show step count using builtin code
#define USE_HEALTH
//#define UPDATE_HEALTH_ON_ACTIVITY  /* If not set, only updates step count display once per minute */

#define PERCENT_WIDTH(x) (PBL_DISPLAY_WIDTH * x / 1000)  // where x is a 1000th. E.g. x=18 == 1.8%, x=1000 == 100%
#define PERCENT_HEIGHT(x) (PBL_DISPLAY_HEIGHT * x / 1000)  // where x is a 1000th. E.g. x=18 == 1.8%, x=1000 == 100%

// As of 2026-03-14, known screen resolutions are:
//   * 144x168 - original rectangle
//   * 180x180 - original Round
//   * 200x228 - Pebble Time 2 (emery)
//   * 260x260 - Pebble Round 2 (Gabbro)
//
// Consult https://developer.repebble.com/guides/tools-and-resources/hardware-information/

#define DRAW_BATTERY
#if PBL_DISPLAY_HEIGHT != 228  // 200x228 Pebble Time 2 (emery)  // FIXME this is basically an EMERY check :-(
    #define DRAW_SMALL_BATTERY
#endif

#ifndef FONT_CLOCK_HEIGHT
    #define FONT_CLOCK_HEIGHT 0  // should be pixel height of FONT_NAME or FONT_SYSTEM_NAME (default 49) which ever is active
#endif

// use full width, so that centering works as expected
#define CLOCK_POS GRect(0, PERCENT_HEIGHT(500) - (FONT_CLOCK_HEIGHT / 2), PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT)
#define BT_POS GRect(0, PERCENT_HEIGHT(800), PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT)

//#define QUIET_TIME_IMAGE RESOURCE_ID_IMAGE_QUIET_TIME
//#define QUIET_TIME_IMAGE_GRECT GRect(20, 20, 20, 20)  // Example assumes a 20x20 image

#define HEALTH_POS GRect(0, 40, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT)


// See https://developer.rebble.io/guides/best-practices/building-for-every-pebble/#available-defines-and-macros for hardware specific defines
#ifdef PBL_ROUND
    #define QUIET_TIME_POS  GRect(0, PERCENT_HEIGHT(100), PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT)

    #define DATE_ALIGN GTextAlignmentCenter
    #define DATE_POS GRect(0, PERCENT_HEIGHT(666) /* TODO font math... */, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT) /* probably taller than really needed */

    #define BAT_ALIGN GTextAlignmentCenter
    #ifdef DRAW_BATTERY
        #define BAT_POS GRect(85, 10, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT) /* probably taller than really needed */
    #else
        #define BAT_POS GRect(0, 140, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT) /* probably taller than really needed */
    #endif /* DRAW_BATTERY */
#else /* PBL_RECT */
    #define QUIET_TIME_POS  GRect(20, 20, 60, 60)
    #if PBL_DISPLAY_HEIGHT == 228  // 200x228 Pebble Time 2 (emery)
        // below untested
        // FIXME below should be lower...
        #define DATE_POS GRect(-5, 205, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT) /* probably taller than really needed */
        #ifdef DRAW_BATTERY
            #define BAT_POS GRect(5, 215, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT)
        #else
            #define BAT_POS GRect(0, 205, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT) /* probably taller than really needed */
        #endif /* DRAW_BATTERY */
    #else  //  144x168  Original pebbles; Pebble Classic (aplite), Pebble Time (basalt), Pebble 2 (diorite), Pebble 2 Duo (flint)
        #define DATE_POS GRect(0, 140, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT) /* probably taller than really needed */
        #ifdef DRAW_BATTERY
            #define BAT_POS GRect(5, 150, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT)
        #else
            #define BAT_POS GRect(0, 140, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT) /* probably taller than really needed */
        #endif /* DRAW_BATTERY */
    #endif  // end of original rectangle size
#endif /* end of Round or rectangle */

/* for screen shots and font testing
#define DEBUG  // If set will update each second and use seconds as minutes for checking updates (not the best for screenshots)
#ifdef DEBUG
#define TICK_HANDLER_INTERVAL SECOND_UNIT
#endif // DEBUG

#define DEBUG_TIME
//#define DEBUG_TIME_SCREENSHOT  // NOTE if set, will turn on backlight BUT also only show a single time// ensure quiet time and bluetooth disconnection info is shown
// ensure quiet time and bluetooth disconnection info is shown
#ifndef quiet_time_is_active  // so not aplite
#define quiet_time_is_active() true  // DEBUG!
#else
#undef quiet_time_is_active  // See if we can force this to always be true
#define quiet_time_is_active() true  // DEBUG!
#endif
#define bluetooth_connection_service_peek() false  // DEBUG!
*/
