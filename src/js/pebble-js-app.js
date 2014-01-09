function configurePebble () {
	var config = {
		foreground: window.localStorage.foreground,
		date: window.localStorage.date,
		statusBar: window.localStorage.statusBar,
		decimalDigits: window.localStorage.decimalDigits,
	};
	Pebble.sendAppMessage(config);
}

Pebble.addEventListener("ready",
	function  () {
		if(window.localStorage.version === null) {
			window.localStorage.version = 1;
			window.localStorage.foreground = "white";
			window.localStorage.date = "mmddyy";
			window.localStorage.statusBar = "off";
			window.localStorage.decimalDigits = "off";
		}
		configurePebble();
	}
);

Pebble.addEventListener("showConfiguration",
  function() {
  		var config = {
  			foreground: window.localStorage.foreground,
  			date: window.localStorage.date,
  			statusBar: window.localStorage.statusBar,
  			decimalDigits: window.localStorage.decimalDigits
  		};
  		var url = "http://dl.dropboxusercontent.com/u/19746203/BinaryClock.html";
  		console.log(url + "?q=" + encodeURIComponent(JSON.stringify(config)));
  		Pebble.openURL(url + "?q=" + encodeURIComponent(JSON.stringify(config)));
  }
);
	
Pebble.addEventListener("webviewclosed",
  function(e) {
  	var response = JSON.parse(decodeURIComponent(e.response));
  	window.localStorage.foreground = response.foreground;
  	window.localStorage.date = response.date;
  	window.localStorage.statusBar = response.statusBar;
  	window.localStorage.decimalDigits = response.decimalDigits;
    configurePebble();
  }
);