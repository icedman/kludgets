// calculator
widget.calculator = new Object;
widget.calculator.evaluateExpression = function(exp, param)
{
	if (exp == "decimal_string" || exp == "thousands_separator")
	{
		gDecimalSeparator = ".";
		gDecimalCode = gDecimalSeparator.charCodeAt(0);
		gDecimalString = "decimal";
		gThousandsSeparator = ",";
	}
	return eval(exp);
}

// weather
widget.closestCity = function()
{
	return null;
}

// KeychainHelper (gmail widget)
KeychainHelper = new Object;
KeychainHelper.passwordForApplication = function(key, id)
{
	return System.decrypt(Kludget.Settings.read(key + id, ""));
}
KeychainHelper.setPasswordForApplication = function(key, id, passwd)
{
	Kludget.Settings.write(key + id, System.encrypt(passwd));
}
KeychainHelper.systemVersion = function()
{
	return "1.0.6";
}
// KeyChainAccess (mailchecker)
KeyChainAccess = KeychainHelper;
KeyChainAccess.setAppName = function(appname)
{
	this.app = appname;
}
KeyChainAccess.loadPassword = function(user)
{
	return this.passwordForApplication(this.app, user);
}
KeyChainAccess.savePassword = function(user, passwd)
{
	this.setPasswordForApplication(this.app, user, passwd);
}
KeyChainAccess.setDebugOn = function(debug)
{}

// roundplugin (converter)
RoundPlugin = new Object;
RoundPlugin.logMessage = function(msg) { alert(msg); }
RoundPlugin.getFormattedValue = function(v)
{
	var prec = 1;
	for(var j = 0; j<the_precision; j++)
		prec *= 10;
	return Math.round(v * prec)/prec;
}
RoundPlugin.getNumericValue = function(v) { return this.getFormattedValue(v); }
RoundPlugin.getSeparators = function() { return ",."; }