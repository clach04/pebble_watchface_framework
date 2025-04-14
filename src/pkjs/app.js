var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

// override send config, in case want to modify (change types) values or debug
Pebble.addEventListener('webviewclosed', function(e) {
    console.log('webviewclosed entry');
    if (e && !e.response) {
        return;
    }

    var dict = clay.getSettings(e.response);
    //console.log(dict.VIBRATE_ON_DISCONNECT);

    console.log('e string config data length=' + JSON.stringify(e).length);
    console.log('dict config data length=' + JSON.stringify(dict).length);
    console.log(JSON.stringify(e));  // NOTE possibly secret leakage if adding secrets to config
    console.log(JSON.stringify(dict));  // NOTE possibly secret leakage if adding secrets to config

    // Send settings values to watch side
    console.log('DEBUG pre-send');
    Pebble.sendAppMessage(dict, function(e) {
        console.log('DEBUG success send');
        console.log('Sent config data to Pebble');
    }, function(e) {
        console.log('DEBUG FAIL send');
        console.log('Failed to send data options to Pebble. Error: ' + JSON.stringify(e));  // NOTE possibly secret leakage if adding secrets to config
    });
    console.log('DEBUG post-send');
});
