#pragma once

#define USE_GENERIC_MAIN
#undef REMOVE_LEADING_ZERO_FROM_TIME  /* 12 hour display will not have a leading "0" if this is defined */
#undef FONT_NAME
#undef FONT_SYSTEM_NAME  /* the default font system will be used */
#undef DEBUG_TIME


#define CLOCK_POS GRect(0, 52, 144, 168) /* probably taller than really needed */
#define BT_POS GRect(0, 120, 144, 168) /* probably taller than really needed */
#define DATE_POS GRect(0, 140, 144, 168) /* probably taller than really needed */
#define BAT_POS GRect(0, 140, 144, 168) /* probably taller than really needed */

