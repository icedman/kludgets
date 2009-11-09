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