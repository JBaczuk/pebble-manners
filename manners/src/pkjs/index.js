var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
  };
  
function getBTCPrice() {
    // Construct URL
    var url = 'https://api.gdax.com/products/BTC-USD/ticker';

    // Send request
    xhrRequest(url, 'GET', 
    function(responseText) {
        // responseText contains a JSON object
        var json = JSON.parse(responseText);
        // Assemble dictionary using our keys
        var dictionary = {
            'BTC_PRICE': json.price
        };
        
        // Send to Pebble
        Pebble.sendAppMessage(dictionary,
            function(e) {
            console.log('BTC info sent to Pebble successfully!');
            },
            function(e) {
            console.log('Error sending weather info to Pebble!');
            }
        );
    });
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
function(e) {
  console.log('PebbleKit JS ready!');
  getBTCPrice();
}
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
function(e) {
  console.log('AppMessage received!');
  getBTCPrice();
}                     
);