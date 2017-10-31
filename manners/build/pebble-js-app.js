/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId])
/******/ 			return installedModules[moduleId].exports;
/******/
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			exports: {},
/******/ 			id: moduleId,
/******/ 			loaded: false
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.loaded = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(0);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */
/***/ (function(module, exports, __webpack_require__) {

	__webpack_require__(1);
	module.exports = __webpack_require__(2);


/***/ }),
/* 1 */
/***/ (function(module, exports) {

	(function(p) {
	  if (!p === undefined) {
	    console.error('Pebble object not found!?');
	    return;
	  }
	
	  // Aliases:
	  p.on = p.addEventListener;
	  p.off = p.removeEventListener;
	
	  // For Android (WebView-based) pkjs, print stacktrace for uncaught errors:
	  if (typeof window !== 'undefined' && window.addEventListener) {
	    window.addEventListener('error', function(event) {
	      if (event.error && event.error.stack) {
	        console.error('' + event.error + '\n' + event.error.stack);
	      }
	    });
	  }
	
	})(Pebble);


/***/ }),
/* 2 */
/***/ (function(module, exports) {

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

/***/ })
/******/ ]);
//# sourceMappingURL=pebble-js-app.js.map