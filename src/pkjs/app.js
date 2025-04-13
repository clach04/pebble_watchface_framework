var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

// override send config, in case want to modify (change types) values or debug
Pebble.addEventListener('webviewclosed', function(e) {
    if (e && !e.response) {
        return;
    }

    var dict = clay.getSettings(e.response);
    //console.log(dict.VIBRATE_ON_DISCONNECT);

    console.log('string config data length=' + JSON.stringify(e).length);
    // Send settings values to watch side
    Pebble.sendAppMessage(dict, function(e) {
        console.log('Sent config data to Pebble');
    }, function(e) {
        console.log('Failed to send config data!');
        console.log(JSON.stringify(e));  // NOTE possibly secret leakage if adding secrets to config
    });
});
