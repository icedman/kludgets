
DEBUG = function(m) {
	try {
		console.log(m);
	} catch (e) {
	}
	System.log(kludget.identifier + "::DEBUG " + m);
}

alert = function(m) {
	// todo
	DEBUG(m);
}
