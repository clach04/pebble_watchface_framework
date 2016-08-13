Watchface Framework
===================

Working, ready to build Bare-Bones watch face for Pebble (OG/Classic, Time, and Round) Aplite, Basalt, and Chalk. With configuration and settings support via [Clay](https://github.com/pebble/clay).

All the examples below use the same framework.

<img src="https://raw.githubusercontent.com/clach04/watchface_JupiterMass/master/screenshots/basalt_charging.png" alt="NR7 Pebble Time color screenshot showing charging in progress"> 
<img src="https://raw.githubusercontent.com/clach04/watchface_JupiterMass/master/screenshots/chalk_round.png" alt="NR7 Pebble Time Round color screenshot">
<img src="https://raw.githubusercontent.com/clach04/watchface_CapNion/master/screenshots/basalt_emu_screenshot_001.png" alt="Capinion Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/clach04/watchface_Paragade/master/screenshots/basalt_paragon_001.png" alt="Paragade Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/clach04/watchface_Paragade/master/screenshots/aplite_renegade_001.png" alt="Paragade Original Pebble Mono screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/132x132_bigtime_at_bottom.png" alt="Spawn Big Time  Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/144x144_time_at_bottom.png" alt="Spawn Small Time  Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/basic_watchface.png" alt="Spawn Big Time and Info  Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/120x120_spawn_battery_text_and_date.png" alt="Spawn with date/time screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/120x120_spawn_battery_drawn_and_date.png" alt="Spawn with date/time screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/120x120_spawn_battery_text_and_date_bw.png" alt="Spawn with date/time screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/120x120_spawn_battery_text_and_date_br.png" alt="Spawn with date/time screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_simple_step/screenshots/aplite_no_step_count.png" alt="Aplite Simple Step (no steps) screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_simple_step/screenshots/mono_step_count.png" alt="Basalt Mono Simple Step screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_simple_step/screenshots/mono_chalk_step_count.png" alt="Chalk Mono Simple Step screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_colony/images/180x180_chalk.png" alt="Colony Chalk screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_colony/images/132x132_basalt.png" alt="Colony Basalt screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_colony/images/132x132_aplite.png" alt="Colony Aplite screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_storm_trooper/screenshots/vector_outline_white.png" alt="StarWars Stormtrooper screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_storm_trooper/screenshots/vector_opaque_black.png" alt="StarWars Stormtrooper screenshot">


Instructions
------------

  1. Copy (or clone) into your project. Click https://cloudpebble.net/ide/import/github/clach04/watchface_framework/ to import
  2. Create a new uuid (in CloudPebble generate one with a single button push under settings) - see appinfo.json
  3. Compile and run!

Then start adding options and resources. By default the empty framework will:

  * Display date - using system font
  * Display time, updating once per minute- using system font
  * Display Battery power
  * Display notice when Bluetooth is disconnected
  * On Basalt and higher (e.g. Chalk) show step count for today
  * Have a config/settings option on Phone to configure:
      * Time/date/etc. text color
      * Background color
      * Whether to vibrate on Bluetooth disconnect (default is do *not* vibrate)

There is no default icon, add a 28x28 resource and declare it as an icon, see http://developer.getpebble.com/guides/pebble-apps/resources/image-resources/

Additional options
------------------

In an ideal situation, `watchface.c` and `watchface.h` should not need editing *ever*. There may be cases where `main.c` needs editing. Most of the time `watch_config.h` is the only file that will need editing. `watch_config.h` options:

  * If `DEFAULT_TIME_COLOR` is defined it will be used for the default time color.
  * If `DEFAULT_BACKGROUND_COLOR` is defined it will be used for the default background time color.
  * If an image is present under resources and defined as `BG_IMAGE`, it will be used as a background image. For Basalt, image transparency is honored.
  * If a font  present under resources and  defined as `FONT_NAME`, it will be used for displaying the time. E.g. `#define FONT_NAME RESOURCE_ID_.....`
  * If `FONT_SYSTEM_NAME` is defined to a system font name and `FONT_NAME` is not defined, that system font will be used for time display
  * if `REMOVE_LEADING_ZERO_FROM_TIME` is defined, and the watch is configured for 12 hour format display, the leading zero "0" will be removed for times in the morning.
  * `CLOCK_POS`, `BT_POS, DATE_POS`, and `BAT_POS` to change the on screen position of Time, Bluetooth disconnect message, Date, and battery power.
  * `FONT_BT_SYSTEM_NAME`, `FONT_BAT_SYSTEM_NAME`, and `FONT_DATE_SYSTEM_NAME` can override the system font used.
      * if `USE_TIME_FONT_FOR_DATE` is defined, the date is displayed using the same font as used for time
  * `BT_ALIGN`, `BAT_ALIGN`, and `TIME_ALIGN` are used to change text alignment.
  * If `BLUETOOTH_DISCONNECTED_STR` is defined, this text will be displayed for the Bluetooth disconnect message.
  * If `BT_DISCONNECT_IMAGE` is defined, this resource image will be displayed on bluetooth disconnect.
      * `BT_DISCONNECT_IMAGE_GRECT` can be used to position the image
  * If `DATE_FMT_STR` is defined it will be used for the format of the date text.
  * If `BAT_FMT_STR` is defined it will be used for the format of the battery power text.
      * If `DRAW_SMALL_BATTERY` is defined a small gauge will be used.
  * If `DRAW_BATTERY` is defined a graphical gauge will be used instead of text
  * `NO_BLUETOOTH`, `NO_BATTERY`, and `NO_DATE` will disable display of bluetooth disconnect, battery status, and date.
  * If `USE_HEALTH` is defined step counts will be displayed. Pebble Time and later only.
      * If `UPDATE_HEALTH_ON_ACTIVITY` is set, step count is updated when the Pebble Health service has an update. If not set, step count is updated once per minute.
      * `HEALTH_POS` is a GRect()
      * `HEALTH_FMT_STR` is the format of the string to display. `MAX_HEALTH_STR` should be updated if `HEALTH_FMT_STR` is set.

Examples
--------

  * Simple text watch, with step count https://github.com/clach04/watchface_simple_step - screenshots https://github.com/clach04/watchface_simple_step/wiki
  * https://github.com/clach04/watchface_JupiterMass - Black and White, Color, and Round support
      * https://github.com/clach04/watchface_JupiterMass/blob/master/src/watch_config.h + the resources (font and image) that make this different from the basic template)
  * https://github.com/clach04/watchface_spawn/wiki - has a number of branches showing different ideas and formats for a watchface with different round image sizes (leaving different amounts of space) and watch_config.h changes. Uses builtin system font and a single PNG image (with transparency). Also see https://github.com/clach04/watchface_colony/wiki. To import and demo these:
      * https://cloudpebble.net/ide/import/github/clach04/watchface_spawn/120x120_bigtime_at_bottom_battery_and_date
      * https://cloudpebble.net/ide/import/github/clach04/watchface_spawn/132x132_bigtime_at_bottom
      * https://cloudpebble.net/ide/import/github/clach04/watchface_spawn/144x144_time_at_bottom
      * https://cloudpebble.net/ide/import/github/clach04/watchface_spawn/basic_watchface
  * https://github.com/clach04/watchface_CapNion
      * https://github.com/clach04/watchface_CapNion/blob/master/src/watch_config.h + the resources (font and image) that make this different from the basic template)
  * https://github.com/clach04/watchface_Paragade - NOTE slightly more complicated than JupiterMass and CapNion as it has:
      * a custom ticker
      * setup/cleanup
      * uses a package dependency on the excellent https://github.com/ygalanter/EffectLayer library
      * See https://github.com/clach04/watchface_ParaGade/blob/master/src/watch_config.h and https://github.com/clach04/watchface_ParaGade/blob/master/src/main.c

Simple Tutorial 1
-----------------

1. Import into CloudPebble, click https://cloudpebble.net/ide/import/github/clach04/watchface_framework/ to import
2. Edit `watch_config.h` at the bottom, add the following lines:

        #define TICK_HANDLER_INTERVAL SECOND_UNIT  / Update once per second
        #define TIME_FMT_STR_24H "%R:%S"  // Display seconds
        #define TIME_FMT_STR_12H "%I:%M:%S"  // Display seconds, produces leading zero for hour,minute, and seconds
        #define MAX_TIME_STR "00:00:00"  // Ensure space allocated for string representation of time
        #define FONT_SYSTEM_NAME FONT_KEY_ROBOTO_CONDENSED_21  // Use a medium sized system  font

This will show seconds (the `%S` in both formats) and update once a second (by setting `TICK_HANDLER_INTERVAL` to update once a second). `MAX_TIME_STR` is updated to ensure space is allocated for both the colon and the two digits.

Simple Tutorial 2
-----------------

1. Import into CloudPebble, click https://cloudpebble.net/ide/import/github/clach04/watchface_framework/ to import
2. Add a Resource (lower left corner), load a TTF font, e.g. upload the .ttf files from https://www.fontsquirrel.com/fonts/permanent-marker
3. Name the resource `FONT_50` - 50 means scale to 50 (optional enter in the CHARACTERS field `[0-9:.]` to only use the characters needed for time, makes watchface smaller), Click Save.
4. Edit `watch_config.h` add line `#define FONT_NAME RESOURCE_ID_FONT_50` add after (or replace) '#undef FONT_NAME' to match resource name above
5. Build and run :-)
6. Try experimenting with different font sizes, make use of `#define DEBUG_TIME` to try wide fonts. For example using the permanent-marker font above with size 60 looks fine for some times but not all. DEBUG_TIME quickly tries a number of different times to check sizes.

NOTE there are limitations in the Pebble SDK/Firmware on maximum font size. 60 is the absolute maximum, and for some fonts even 60 is problematic. Very large fonts need to be converted into bitmaps, see https://github.com/pebble-examples/big-time

Notes and other resources
-------------------------

For suggestions on image options and converstion see https://www.reddit.com/r/pebble/comments/3ej5wf/pebble_time_image_conversion_contrast/

Multi-platform support https://developer.getpebble.com/guides/best-practices/multi-platform/
