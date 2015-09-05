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
