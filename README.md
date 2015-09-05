Watchface Framework
===================

Working, ready to build Bare-Bones watch face for Pebble (OG and Time) Aplite and Basalt.

Instructions
------------

  1. Copy (or clone) into your project
  2. Create you own uuid (in CloudPebble generate one with a single button push under settings) - see appinfo.json
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

Additional options
------------------

See `watch_config.h`:

  * If `DEFAULT_TIME_COLOR` is defined it will be used for the default time color.
  * If `DEFAULT_TIME_COLOR` is defined it will be used for thedefault background time color.
  * If an image is present under resources and defined as `BG_IMAGE`, it will be used as a background image. For Basalt, image transparency is honored.
  * If a font  present under resources and  defined as `FONT_NAME`, it will be used for displaying the time.
  * If `FONT_SYSTEM_NAME` is defined to a system font name and `FONT_NAME` is not defined, that system font will be used for time display
  * if `REMOVE_LEADING_ZERO_FROM_TIME` is defined, and the watch is configured for 12 hour format display, the leading zero "0" will be removed for times in the morning.
  * Update CLOCK_POS, BT_POS, DATE_POS, and BAT_POS to change the on screen position of Time, Bluetooth disconnect message, Date, and battery power.
  * `BT_ALIGN`, `BAT_ALIGN`', and `TIME_ALIGN` are used to change alignment.
  * If BLUETOOTH_DISCONNECTED_STR is defined, this text will be displayed for the Bluetooth disconnect message.
  * If `DATE_FMT_STR` is defined it will be used for the format of the date text.
  * If `BAT_FMT_STR` is defined it will be used for the format of the battery power text.
  * If `` is defined it will be used for the
