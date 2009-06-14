
Kludget.setPreferenceForKey = function(value, key)
{
	try {
		if (typeof value == "object")
			value = JSON.stringify(value, null);
	} catch(e) {
		// alert('setPreferenceForKey: ' + e + ':' + key);
	}
	Kludget.Settings.write(key, value);
}

Kludget.preferenceForKey = function(key)
{
	if (!Kludget.Settings.contains(key))
		return null;
	var value = Kludget.Settings.read(key, "");
	try {
		var obj = JSON.parse(value, null);
		if (typeof obj == "object")
			value = obj;
	} catch(e) {
		// alert('preferenceForKey: ' + e + ':' + key);
	}
	return value;
}

Kludget.system = function(cmd, handler)
{
	var cmdObject = new Command;
	cmdObject.execute(cmd, handler);
	return cmdObject;
}

Kludget.setCloseBoxOffset = function(x, y)
{
	alert("setCloseBoxOffset: not implemented");
}

Kludget.openURL = function(url)
{
	System.openURL(url);
}

Kludget.openApplication = function(path)
{
	alert("openApplication: not implemented");
	alert("app: " + path);
}

Kludget.onshow = function() {}
Kludget.onhide = function() {}
Kludget.onstartdrag = function() {}
Kludget.onenddrag = function() {}
Kludget.onremove = function() {}
Kludget.onsettingschanged = function() {}
Kludget.onurlreceived = function(url) {}

Kludget.dispatchMouseLeave = function()
{
	var evt = document.createEvent("MouseEvent");
	evt.initMouseEvent("mouseout", true, true, window, 
			0, 0, 0, -1, -1, 
			false, false, false, false, 
			false, false);
	document.body.dispatchEvent(evt);
}

window.kludget = Kludget;
window.widget = Kludget;
window.Widget = Kludget;