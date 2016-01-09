var URL = 'http://clach04.github.io/pebble/watchface_framework/slate/index.html';

var default_dict = {
    version_settings: 0,  // Bump this when ever config storage changes
    // http://developer.getpebble.com/tools/color-picker/
    background_color: '000000',  // GColorBlack
    time_color: 'FFFFFF',  // GColorWhite
    vibrate_disconnect: 0
};


function force_bool(in_value)
{
    var result=false;

    switch (in_value)
    {
        case true:
        case 'true':
        case 'True':
        case 'TRUE':
        case 1:
        case '1':
        case 'on':
        case 'On':
        case 'ON':
        case 'yes':
        case 'Yes':
        case 'YES':
        case 'Y':
            result = true;
            break;
        default:
            result = false;
            break;
    }
    return result;
}

function getStorageValue(item, default_value){
    var retVal = localStorage.getItem(item);
    //console.log('value' + item + ': ' + String(retVal));
    if (retVal === null || retVal == 'undefined' || retVal == 'null'){
        retVal = default_value;
    }
    return retVal;
}

/**
 * Overwrites obj1's values with obj2's and adds obj2's if non existent in obj1
 * @param obj1
 * @param obj2
 * @returns obj3 a new object based on obj1 and obj2
 * Similar to ES6 Object.assign() but without side effects and works with older js implementations
 * See
 *    * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/assign
 *    * http://stackoverflow.com/questions/171251/how-can-i-merge-properties-of-two-javascript-objects-dynamically
 */
function merge_options(obj1, obj2){
    var obj3 = {};
    var attrname;

    for (attrname in obj1) { obj3[attrname] = obj1[attrname]; }
    for (attrname in obj2) { obj3[attrname] = obj2[attrname]; }
    return obj3;
}

Pebble.addEventListener('showConfiguration', function(e) {
    /* DEBUG test old settings are handled and cleaned up */
    /*
            localStorage.setItem('background_color', '0000FF');
            localStorage.setItem('time_color', 'FFFF00');
            localStorage.setItem('vibrate_disconnect', true);
    */
    /* DEBUG */

    var stored_dict_str = getStorageValue('stored_dict', null);
    console.log('stored_dict_str: ' + stored_dict_str);
    var stored_dict;
    if (stored_dict_str === null)
    {
        stored_dict = {};
    }
    else
    {
        stored_dict = JSON.parse(stored_dict_str);
    }
    console.log('stored_dict: ' + JSON.stringify(stored_dict));
    console.log('default_dict: ' + JSON.stringify(default_dict));

    /* Handle old config - not stored in dict */
    console.log('Checking old (non-dict) config items');
    var old_config_names = ['background_color', 'time_color', 'vibrate_disconnect'];
    for (var i in old_config_names)
    {
        var config_name=old_config_names[i];
        var tmp_setting;
        console.log('i=' + i + '=' + config_name);
        tmp_setting = getStorageValue(config_name, null);
        if (tmp_setting !== null)
        {
            default_dict[config_name] = tmp_setting;
            console.log('about to remove old localStorage item ' + config_name);
            localStorage.removeItem(config_name);
        }
    }

    var configuration = merge_options(default_dict, stored_dict);
    if (Pebble.getActiveWatchInfo)
    {
        /*
        ** SDK v3 or higher.
        ** E.g. Basalt, Chalk, or Aplite with 3.8+
        */
        var info = Pebble.getActiveWatchInfo();
        console.log('Pebble info: ' + JSON.stringify(info));
        configuration.pebble_platform = info.platform;
        configuration.pebble_model = info.model;
        configuration.pebble_language = info.language;
        /*
        ** Useful info:
            platform: aplite|basalt|chalk
            model: qemu_platform_aplite|qemu_platform_basalt|qemu_platform_chalk|pebble_time_black

        Seen in emulator:
        {
            "platform": "aplite",
            "model": "qemu_platform_aplite",
            "firmware": {
                "major": 3,
                "suffix": "",
                "minor": 3,
                "patch": 1
            },
            "language": "en_US"
        }
        {
            "platform": "basalt",
            "model": "qemu_platform_basalt",
            "firmware": {
                "major": 3,
                "suffix": "",
                "minor": 3,
                "patch": 1
            },
            "language": "en_US"
        }
        {
            "platform": "chalk",
            "model": "qemu_platform_chalk",
            "firmware": {
                "major": 3,
                "suffix": "",
                "minor": 3,
                "patch": 1
            },
            "language": "en_US"
        }
        */
    }
    else
    {
        /*
        ** Info NOT available with SDK pre v3. Probably old Aplite.
        ** NOTE emulator with 3.8 SDK for aplite does have this info.
        */
    }
    console.log('configuration: ' + JSON.stringify(configuration));

    var param_array = [];
    var temp_str;
    var config_key;
    for (config_key in configuration)
    {
        console.log('config_key: ' + config_key + '=' + configuration[config_key]);
        temp_str = config_key + '=' + encodeURIComponent(configuration[config_key]);
        param_array.push(temp_str);
    }
    var URL_with_params = URL + '?' + param_array.join('&');
    console.log('Configuration window opened. ' + URL_with_params);
    Pebble.openURL(URL_with_params);
});

Pebble.addEventListener('webviewclosed',
    function(e) {
        console.log('e.response: ' + e.response);
        console.log('e.response.length: ' + e.response.length);
        try {
            var configuration_from_page = JSON.parse(decodeURIComponent(e.response));
            console.log('dictionary to validate ' + JSON.stringify(configuration_from_page));
            /*
            ** configuration_from_page is from an external web site,
            ** this is not trustworthy and so needs validation.
            ** Validation/translation reduces the need for _some_
            ** protection C code on the Pebble.
            */

            /*
            ** As configuration is untrusted and may be missing values,
            ** merge in from default_dict
            */
            var configuration = merge_options(default_dict, configuration_from_page);

            /* Validate config */
            configuration.vibrate_disconnect = force_bool(configuration.vibrate_disconnect) ? 1 : 0;  // Force to int

            /*
            ** even though we don't really trust `configuration`, store it in local phone storage
            ** TODO store (validated) dictionary (below) on phone instead
            **    - key names different
            **    - color settings string (hex) versus int number
            */
            console.log('store config on phone: ' + JSON.stringify(configuration));
            localStorage.setItem('stored_dict', JSON.stringify(configuration));

            // Send to Pebble
            var dictionary = {
              "KEY_TIME_COLOR": parseInt(configuration.time_color, 16),  // TODO color not validated
              "KEY_BACKGROUND_COLOR": parseInt(configuration.background_color, 16),  // TODO color not validated
              "KEY_VIBRATE_ON_DISCONNECT": configuration.vibrate_disconnect
            };

            console.log('dictionary to send to Pebble' + JSON.stringify(dictionary));
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log("Configuration sent to Pebble successfully!");
                },
                function(e) {
                    console.log("Error sending configuration info to Pebble!");
                }
            );
        } catch (ex) {
            // If we have SyntaxError JSON is invalid, anything unknown!?
            if (ex instanceof SyntaxError) {
                console.log('Probably Cancelled');
            } else {
                throw ex;
                // See https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SyntaxError
            }
        }
    }
);
