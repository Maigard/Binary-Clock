function configurePebble () {
	var config = {
		foreground: window.localStorage.foreground,
		background: window.localStorage.background,
		layout: window.localStorage.layout,
		statusBar: window.localStorage.statusBar,
		decimalDigits: window.localStorage.decimalDigits,
	};
	Pebble.sendAppMessage(config);
}

Pebble.addEventListener("ready",
	function  () {
		if(window.localStorage.version === null) {
			window.localStorage.version = 1;
			window.localStorage.foreground = "black";
			window.localStorage.background = "white";
			window.localStorage.layout = "vertical";
			window.localStorage.statusBar = "on";
			window.localStorage.decimalDigits = "off";
		}
	}
);

Pebble.addEventListener("showConfiguration",
  function() {
  		var config = {
  			foreground: window.localStorage.foreground,
  			layout: window.localStorage.layout,
  			statusBar: window.localStorage.statusBar,
  			decimalDigits: window.localStorage.decimalDigits
  		};
  		var url = "http://dl.dropboxusercontent.com/u/19746203/BinaryClock.html";
  		Pebble.openURL(url + "?q=" + encodeURIComponent(JSON.stringify(config)));
  }
);
	
Pebble.addEventListener("webviewclosed",
  function(e) {
  	if (e.response) {
	  	var response = JSON.parse(decodeURIComponent(e.response));
	  	window.localStorage.foreground = response.foreground;
	  	window.localStorage.layout = response.layout;
	  	window.localStorage.statusBar = response.statusBar;
	  	window.localStorage.decimalDigits = response.decimalDigits;
		window.background = response.foreground == "white" ? "black" : "white";
		configurePebble();
	}
  }
);