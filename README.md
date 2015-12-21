Watchface Framework
===================

Working, ready to build Bare-Bones watch face for Pebble (OG/Classic, Time, and Round) Aplite, Basalt, and Chalk. With configuration and settings support.

All the examples below use the same framework.

<img src="https://raw.githubusercontent.com/clach04/watchface_JupiterMass/master/screenshots/basalt_charging.png" alt="NR7 Pebble Time color screenshot showing charging in progress"> 
<img src="https://raw.githubusercontent.com/clach04/watchface_JupiterMass/master/screenshots/chalk_round.png" alt="NR7 Pebble Time Round color screenshot">
<img src="https://raw.githubusercontent.com/clach04/watchface_CapNion/master/screenshots/basalt_emu_screenshot_001.png" alt="Capinion Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/clach04/watchface_Paragade/master/screenshots/basalt_paragon_001.png" alt="Paragade Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/clach04/watchface_Paragade/master/screenshots/aplite_renegade_001.png" alt="Paragade Original Pebble Mono screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/132x132_bigtime_at_bottom.png" alt="Spawn Big Time  Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/144x144_time_at_bottom.png" alt="Spawn Small Time  Pebble Time color screenshot">
<img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/basic_watchface.png" alt="Spawn Big Time and Info  Pebble Time color screenshot">



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
  * If a font  present under resources and  defined as `FONT_NAME`, it will be used for displaying the time.
  * If `FONT_SYSTEM_NAME` is defined to a system font name and `FONT_NAME` is not defined, that system font will be used for time display
  * if `REMOVE_LEADING_ZERO_FROM_TIME` is defined, and the watch is configured for 12 hour format display, the leading zero "0" will be removed for times in the morning.
  * `CLOCK_POS`, `BT_POS, DATE_POS`, and `BAT_POS` to change the on screen position of Time, Bluetooth disconnect message, Date, and battery power.
  * `FONT_BT_SYSTEM_NAME`, `FONT_BAT_SYSTEM_NAME`, and `FONT_DATE_SYSTEM_NAME` can override the system font used.
      * if `USE_TIME_FONT_FOR_DATE` is defined, the date is displayed using the same font as used for time
  * `BT_ALIGN`, `BAT_ALIGN`, and `TIME_ALIGN` are used to change alignment.
  * If `BLUETOOTH_DISCONNECTED_STR` is defined, this text will be displayed for the Bluetooth disconnect message.
  * If `BT_DISCONNECT_IMAGE` is defined, this resource image will be displayed on bluetooth disconnect.
  * If `DATE_FMT_STR` is defined it will be used for the format of the date text.
  * If `BAT_FMT_STR` is defined it will be used for the format of the battery power text.
  * `NO_BLUETOOTH`, `NO_BATTERY`, and `NO_DATE` will disable display of bluetooth disconnect, battery status, and date.

Examples
--------

  * https://github.com/clach04/watchface_JupiterMass - Black and White, Color, and Round support
      * https://github.com/clach04/watchface_JupiterMass/blob/master/src/watch_config.h + the resources (font and image) that make this different from the basic template)
  * https://github.com/clach04/watchface_spawn/wiki - has a number of branches showing different ideas and formats for a watchface with changes only made in watch_config.h. Uses builtin system font and a single PNG image (with transparency) To import and demo these:
      * https://cloudpebble.net/ide/import/github/clach04/watchface_spawn/132x132_bigtime_at_bottom
      * https://cloudpebble.net/ide/import/github/clach04/watchface_spawn/144x144_time_at_bottom
      * https://cloudpebble.net/ide/import/github/clach04/watchface_spawn/basic_watchface
  * https://github.com/clach04/watchface_CapNion
      * https://github.com/clach04/watchface_CapNion/blob/master/src/watch_config.h + the resources (font and image) that make this different from the basic template)
  * https://github.com/clach04/watchface_Paragade - NOTE slightly more complicated than JupiterMass and CapNion as it has a custom ticker (and setup/cleanup)
      * See https://github.com/clach04/watchface_ParaGade/blob/master/src/watch_config.h and https://github.com/clach04/watchface_ParaGade/blob/master/src/main.c

Notes and other resources
-------------------------

For suggestions on image options and converstion see https://www.reddit.com/r/pebble/comments/3ej5wf/pebble_time_image_conversion_contrast/

Multi-platform support https://developer.getpebble.com/guides/best-practices/multi-platform/
