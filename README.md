Pebble Watchface Framework
==========================

https://github.com/clach04/pebble_watchface_framework

Working, ready to build Bare-Bones watch face for Pebble (OG/Classic, Time, Round, and Pebble 2) Aplite, Basalt, Chalk, and Diorite. With configuration and settings support via [Clay](https://github.com/clach04/pebble-clay/tree/clach04_emulator_url_index).

Requires Pebble SDK of some kind current code and WAF script is for SDK 4.5, works with 4.3 (use `pebble sdk list
` to confirm version). Original instructions are for CloudPebble.

For non-developers, check out https://github.com/HarrisonAllen/pebble-watchface-generator (started 2025).

Current code works with local SDK, recommend using a Docker Container, for example:

  * https://github.com/bboehmke/docker-pebble-dev
  * https://github.com/clach04/docker-pebble-dev/wiki has some notes

GitHub Actions note, https://github.com/clach04/pebble_watchface_framework/blob/master/.github/workflows/main.yml includes a pebble build action that runs by default on every push to master! NOTE Downloads are ONLY available when logged into Github, otherwise, the Artifacts "PebblePBW" text is not a link (without any hint as to what is going on).
Kudos to https://github.com/daktak for initial version that the action used here is based on. First seen in https://github.com/mattrossman/forecaswatch2/pull/50

Installation notes for PBW (Android specific, assuming Pebble/Rebble App is installed https://help.rebble.io/setup/):

  * Recommend using [Cx File Explorer](https://play.google.com/store/apps/details?id=com.cxinventor.file.explorer) to "open" it and then select Pebble App.
      * NOTE Android 9 and later also require [File URI Plugin](https://play.google.com/store/apps/details?id=com.alphainventor.plugin.fileuri)
  * Side Loader https://play.google.com/store/apps/details?id=io.rebble.charon
  * Or use Gadget Bridge https://codeberg.org/Freeyourgadget/Gadgetbridge/wiki/Pebble-Getting-Started

All the examples below use the same framework.

<img src="https://raw.githubusercontent.com/clach04/watchface_JupiterMass/master/screenshots/basalt_charging.png" alt="NR7 Pebble Time color screenshot showing charging in progress"> <img src="https://raw.githubusercontent.com/clach04/watchface_JupiterMass/master/screenshots/chalk_round.png" alt="NR7 Pebble Time Round color screenshot"> <img src="https://raw.githubusercontent.com/clach04/watchface_CapNion/master/screenshots/basalt_emu_screenshot_001.png" alt="Capinion Pebble Time color screenshot"> <img src="https://raw.githubusercontent.com/clach04/watchface_Paragade/master/screenshots/basalt_paragon_001.png" alt="Paragade Pebble Time color screenshot"> <img src="https://raw.githubusercontent.com/clach04/watchface_Paragade/master/screenshots/aplite_renegade_001.png" alt="Paragade Original Pebble Mono screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/132x132_bigtime_at_bottom.png" alt="Spawn Big Time  Pebble Time color screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/144x144_time_at_bottom.png" alt="Spawn Small Time  Pebble Time color screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/basic_watchface.png" alt="Spawn Big Time and Info  Pebble Time color screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/120x120_spawn_battery_text_and_date.png" alt="Spawn with date/time screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/120x120_spawn_battery_drawn_and_date.png" alt="Spawn with date/time screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/120x120_spawn_battery_text_and_date_bw.png" alt="Spawn with date/time screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_spawn/images/120x120_spawn_battery_text_and_date_br.png" alt="Spawn with date/time screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_simple_step/screenshots/aplite_no_step_count.png" alt="Aplite Simple Step (no steps) screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_simple_step/screenshots/mono_step_count.png" alt="Basalt Mono Simple Step screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_simple_step/screenshots/mono_chalk_step_count.png" alt="Chalk Mono Simple Step screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_colony/images/180x180_chalk.png" alt="Colony Chalk screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_colony/images/132x132_basalt.png" alt="Colony Basalt screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_colony/images/132x132_aplite.png" alt="Colony Aplite screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_storm_trooper/screenshots/vector_outline_white.png" alt="StarWars Stormtrooper screenshot"> <img src="https://raw.githubusercontent.com/wiki/clach04/watchface_storm_trooper/screenshots/vector_opaque_black.png" alt="StarWars Stormtrooper screenshot">

Instructions
------------

### GitHub CodeSpace Cloud SDK

#### Quick GitHub CodeSpace Cloud SDK

  1. Open https://github.com/clach04/pebble_watchface_framework/ in web browser
  2. Click the green "Use this template" button (top right), "Open in a codespace"
  3. Wait.....
  4.  Open source file (e.g. watchface.c), see the problems pane with a couple
    of errors. Right-click on the first one,
    `#include errors detected. Please update your includePath. Squiggles are disabled for this translation unit (/workspaces/codespaces-pebble/myfirstproject/src/c/myfirstproject.c).`,
    right click and edit include path `Edit "includePath" setting`.
    Then close the new edit view.
    This will create a .vscode file with the correct path (`${workspaceFolder}/**`); a preset one is not
    included to avoid local SDK / VScode conflicts
    (TODO: consider updating the setup script to handle this).
  5. Click the Pebble button in the left-hand bar, and use run buttons to build and run in the emulator.

#### Longer GitHub CodeSpace Cloud SDK

These are generic instructions and work for any codespace template.

  1. Open https://github.com/codespaces
  2. Find green button, "New codespace"
  3. Select repository (or copy/paste `clach04/pebble_watchface_framework`)
  4. Adjust as needed then click green "Create codespace" button
  5. Wait.....
  6. Open source file (e.g. watchface.c), see problems pane fill with a couple
    of errors. Right click on first one,
    `#include errors detected. Please update your includePath. Squiggles are disabled for this translation unit (/workspaces/codespaces-pebble/myfirstproject/src/c/myfirstproject.c).`,
    right click and edit include path `Edit "includePath" setting`.
    Then close new edit view.
    This will create a .vscode file with correct path, a preset one not not
    included so as to avoid local SDK conflicts
    (TODO consider updating setup script to handle this).
  7. Click Pebble button in left hand bar, and use run buttons to build and run in emulator.

Alternatively - less typing/clicking:

  1. Open https://github.com/clach04/codespaces-pebble
  2. Find green button, "Use this template", click dropdown triangle, Open in a codespace
  3. Open source file (e.g. watchface.c), see problems pane fill with a couple of errors. Right click on first one, `#include errors detected. Please update your includePath. Squiggles are disabled for this translation unit (/workspaces/codespaces-pebble/myfirstproject/src/c/myfirstproject.c).`, right click and edit include path `Edit "includePath" setting`. Then close new edit view.
  4. Follow the instructions in https://developer.repebble.com/sdk/cloud
  5. Update the UUID in `package.json`. Also see section "Simple Tutorial 2" and "Simple Tutorial 3" later in readme

Stop/Delete codespaces via https://github.com/codespaces

## Edit and customize

Start adding options and resources. By default, the empty framework will:

  * Display date - using system font
  * Display time, updating once per minute- using system font
  * Display Battery power
  * Display notice when Bluetooth is disconnected
  * On Basalt and higher (e.g. Chalk) show step count for today
  * Have a config/settings option on Phone to configure:
      * Time/date/etc. text color
      * Background color
      * Whether to vibrate on Bluetooth disconnect (default is do *not* vibrate), honoring Quiet Time mode

There is no default icon, add a 25x25 resource and declare it as an icon, see https://developer.rebble.io/developer.pebble.com/guides/migration/migration-guide-4/index.html
Additional options

https://www.stef.be/dpaint/ is a good web/cloud-based bitmap editor. Upload images to `resources` directory and update `package.json`. See linked examples/samples for images.

Also, you can use a local SDK.

------------------

## Edit Hints

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
  * If `QUIET_TIME_IMAGE` is defined as a bitmap resource identifier it will be displayed when quiet time is active.
      * Optionally set `QUIET_TIME_IMAGE_GRECT` to location/size
  * `NO_BLUETOOTH`, `NO_BATTERY`, and `NO_DATE` will disable display of bluetooth disconnect, battery status, and date.
  * If `USE_HEALTH` is defined step counts will be displayed. Pebble Time and later only.
      * If `UPDATE_HEALTH_ON_ACTIVITY` is set, step count is updated when the Pebble Health service has an update. If not set, step count is updated once per minute.
      * `HEALTH_POS` is a GRect()
      * `HEALTH_FMT_STR` is the format of the string to display. `MAX_HEALTH_STR` should be updated if `HEALTH_FMT_STR` is set.
  * If `USE_TIME_MACHINE` is defined https://github.com/MorrisTimm/pebble-time-machine will be used ('dependencies' in package.json should be filled in, known to work with version 1.0.2).

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
      * uses a package dependency on the excellent https://github.com/ygalanter/pebble-effect-layer library
      * See https://github.com/clach04/watchface_ParaGade/blob/master/src/watch_config.h and https://github.com/clach04/watchface_ParaGade/blob/master/src/main.c
  * https://github.com/clach04/pebble-watchface_starwars_rebel_imperial similar to Paragade (no screen shots, but does have downloads under GitHub actions and Releases)
  * https://github.com/clach04/pebble-watchface_storm_trooper has screen shots and and Release downloads
  * https://github.com/clach04/watchface_rota_minute analog/digital hybrid
  * https://github.com/clach04/pebble_tz multiple timezones, not pretty but very functional

Simple Tutorial 1
-----------------

 1. clone

        git clone https://github.com/clach04/pebble_watchface_framework.git

    TODO consider editing `package.json` and changing:
      * `name`
      * `displayName`
      * `uuid`

 2. build

        pebble build
        pebble analyze-size

 3. Run in emulator

        pebble install --emulator aplite --logs  # OG Pebble
        pebble build && pebble install --emulator aplite --logs  # build then emulate OG Pebble
        pebble install --emulator basalt --logs  # Pebble Time
        pebble install --emulator chalk --logs  # Pebble Round
        pebble install --emulator diorite --logs  # Pebble 2
        pebble install --emulator emery --logs  # Pebble Time 2

    Start emulator, requires a web browser (NOTE potential issues with Clay).

        pebble emu-app-config

    Kill / clean up:

        pebble kill
        pebble wipe
        pebble clean

 3. Run on Pebble watch, connected via Developer Connection
    TODO document phone steps

        pebble install --phone IP_ADDRESS_HERE --logs
        pebble build && pebble install --phone IP_ADDRESS_HERE --logs
        pebble logs --color --phone PHONE_IP

## Core Devices App

Enable remote install / logs via:

 1. Settings, enable Use LAN devloper connection
 2. Device view, enable Dev Connection

BOTH are required for local LAN connections.
The last will show IP address to use.
Ensure phone and dev machine are using the same access point.

Simple Tutorial 2
-----------------

1. Import into Cloud CodeSpace - see Instructions section
2. Edit `watch_config.h` at the bottom, add the following lines:

        #define TICK_HANDLER_INTERVAL SECOND_UNIT  // Update once per second
        #define TIME_FMT_STR_24H "%R:%S"  // Display seconds
        #define TIME_FMT_STR_12H "%I:%M:%S"  // Display seconds, produces leading zero for hour,minute, and seconds
        #define MAX_TIME_STR "00:00:00"  // Ensure space allocated for string representation of time
        #define FONT_SYSTEM_NAME FONT_KEY_ROBOTO_CONDENSED_21  // Use a medium sized system  font

This will show seconds (the `%S` in both formats) and update once a second (by setting `TICK_HANDLER_INTERVAL` to update once a second). `MAX_TIME_STR` is updated to ensure space is allocated for both the colon and the two digits.

Simple Tutorial 3
-----------------

1. Import into Cloud CodeSpace - see Instructions section
2. Add a Resource (lower left corner), load a TTF font, e.g. upload the .ttf files from https://www.fontsquirrel.com/fonts/permanent-marker
3. Name the resource `FONT_50` - 50 means scale to 50 (optional enter in the CHARACTERS field `[0-9:.]` to only use the characters needed for time, makes watchface smaller), Click Save.
4. Edit `watch_config.h` add line `#define FONT_NAME RESOURCE_ID_FONT_50` add after (or replace) '#undef FONT_NAME' to match resource name above
5. Build and run :-)
6. Try experimenting with different font sizes, make use of `#define DEBUG_TIME` to try wide fonts. For example using the permanent-marker font above with size 60 looks fine for some times but not all. DEBUG_TIME quickly tries a number of different times to check sizes. Also see `USE_TIME_MACHINE` notes.

NOTE there are limitations in the Pebble SDK/Firmware on maximum font size. 60 is the absolute maximum, and for some fonts even 60 is problematic. Very large fonts need to be converted into bitmaps, see https://github.com/pebble-examples/big-time

Notes and other resources
-------------------------

For suggestions on image options and conversion see https://www.reddit.com/r/pebble/comments/3ej5wf/pebble_time_image_conversion_contrast/

Multi-platform support https://developer.rebble.io/developer.pebble.com/guides/best-practices/index.html

SDK API https://developer.rebble.io/developer.pebble.com/docs/index.html

Pebble developer wiki https://github.com/pebble-dev/wiki/wiki

