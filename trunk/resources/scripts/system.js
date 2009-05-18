var Command = function() {
	this.pid = 0;
	this.outputString = "";
	this.errorString = "";
	this.status = 0;
	this.handler = 0;
}

Command.prototype.cancel = function()
{
	System.cancel(this.pid);
}

Command.prototype.close = function()
{
	System.close(this.pid);
}

Command.prototype.write = function(text)
{
	System.write(text, this.pid);
}

Command.prototype.update = function()
{
	var prevError = this.errorString;
	var prevOutput = this.outputString;
	
	if (System.hasError(this.pid))
	{
		this.errorString = System.error(this.pid);
		this.onreaderror(this.errorString);
	}
	if (System.hasOutput(this.pid))
	{
		this.outputString = System.output(this.pid);
		this.onreadoutput(this.outputString);
	}
	this.status = System.status(this.pid);
	
	if (this.handler == 0)
	{
		this.outputString = this.outputString.replace(prevOutput, "");
		this.errorString = this.errorString.replace(prevError, "");
		this.outputString = prevOutput + this.outputString;
		this.errorString = prevError + this.errorString;
	}
}

Command.prototype.execute = function(cmd, handler)
{
	if (handler)
		this.handler = handler;

	this.pid = System.execute(cmd);
	window["_syscmd_" + this.pid] = this;
	
	if (this.handler == 0)
	{
		System.cancel(this.pid, 60000);
		this.update();
	}
}

// events
Command.prototype.onfinish = function()
{
	this.update();
	if (this.handler)
		this.handler(this);
}

Command.prototype.onreadoutput = function()
{}

Command.prototype.onreaderror = function()
{}