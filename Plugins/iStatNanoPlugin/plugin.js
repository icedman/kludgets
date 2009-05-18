
function changeLabel(id, newLabel)
{
	var find = document.getElementById(id);
	if (find)
		find.previousSibling.innerHTML = newLabel;
}

iStatNano.cpuUsage = function()
{
	var cpu = iStatNano.cpu.getUsage();
	
	var sys = cpu.system;
	var user = cpu.user;
	var nice = 0;
	
	var load = sys + user;
	var idle = 100 - load;
	if (idle < 0)
		idle = 0;
		
	var mainData = new Array(
		cpu.system, 
		cpu.user,
		nice,
		idle,
		load);
		
	var summary;
		
	if (cpu.value("system2")>0)
	{
		var system1 = cpu.system1;
		var user1 = cpu.user1;
		var system2 = cpu.system2;
		var user2 = cpu.user2;
		summary = new Array(
			Array(system1 + user1, system1, user1, 0),
			Array(system2 + user2, system2, user2, 0)
		);
	} else {
		summary = new Array(
			Array(load, sys, user, nice)
		);
	}
		
	var ret = new Array(mainData, summary, 0);
	
	return ret;
}

iStatNano.memoryUsage = function()
{
	iStatNano.memory.update();

	var mem = iStatNano.memory.getUsage();
	var ret = new Array(
		mem.free,
		mem.used,
		mem.freeVirtual, // active
		"", // inactive
		mem.usedVirtual, // wired
		mem.load, // percentage
		mem.paged, // pageins
		"", // pageouts
		0, // ?
		mem.totalPage, // swap
		0, //mem.value("wired%"),
		mem.load //mem.value("active%")
	);
	
	changeLabel('mem_pageins', 'Paged: ');
	changeLabel('mem_pageouts', '');
	changeLabel('mem_wired', 'Virtual: ');
	changeLabel('mem_active', 'Free: ');
	changeLabel('mem_inactive', '');

	return ret;
}

iStatNano.diskUsage = function()
{
	iStatNano.disk.update();
	
	var ret = new Array;
	var disks = iStatNano.disk.getDiskCount();
	
	for(i = 0; i<disks; i++)
	{
		var disk = iStatNano.disk.getDisk(i);
		
		ret[i] = new Array(
			disk.name,
			100 - disk.usage,
			disk.total,
			disk.free,
			disk.path,
			disk.icon,
			0);
	}
	
	return ret;
}

iStatNano.interfaces = 0;

iStatNano.network = function()
{
	var ret = new Array;
	
	if (this.interfaces == 0)
	{
		var objRes = widget.system('typeperf -qx "\Network Interface"');
		var netNames = objRes.outputString.match(/\(.*\)/g);
		this.interfaces = new Array();
		for(var n in netNames)
		{
			if (this.interfaces[0] == netNames[n])
				break;
			if (netNames[n].indexOf("Loopback") != -1)
				continue;
			this.interfaces[this.interfaces.length] = netNames[n];
			iStatNano.net.addInterface(netNames[n]);
		}
	}
	
	iStatNano.net.update();
	 
	var ips = iStatNano.net.getInterfaceCount();
	
	for(i = 0; i<ips; i++)
	{
		var net = iStatNano.net.getInterface(i);
		var name = net.name; 
		if (name.length > 22)
			name = name.substr(0, 19) + "...";
		ret[i] = new Array(
			name,
			"",
			"",
			net.received,
			net.sent,
			net.totalReceived,
			net.totalSent);
	}
	
	return ret;
}

iStatNano.temps = function()
{
	var ret = new Array(0);
	return ret;
}

iStatNano.fans = function()
{
	var ret = new Array(0);
	return ret;
}

iStatNano.batteries = 0;

iStatNano.battery = function()
{
	var ret = new Array;
	
	if (this.batteries == 0)
	{
		var objRes = widget.system('typeperf -qx "\BatteryStatus"');
		var batNames = objRes.outputString.match(/\(.*\)/g);
		this.batteries = new Array();
		for(var n in batNames)
		{
			if (this.batteries[0] == batNames[n])
				break;
			if (batNames[n].indexOf("Loopback") != -1)
				continue;
			this.batteries[this.batteries.length] = batNames[n];
		}
	}
	
	for(var b in this.batteries)
	{
		iStatNano.bat.addBattery(this.batteries[b]);
	}
	
	iStatNano.bat.update();
	 
	var bats = iStatNano.bat.getBatteryCount();
	
	for(i = 0; i<bats; i++)
	{
		var bat = iStatNano.bat.getBattery(i);
		var name = bat.name; 
		var source = "AC Power";
		var health = "Online";
		var time = bat.time;
		
		if (bat.discharge > 0)
		{
			source = "Battery";
			health = "Discharging";
			if (bat.life < 10)
				health = "Critical";
			if (time != "")
				time = time + " remaining";
		} else if (bat.charge > 0)
		{
			health = "Charging";
		}
		
		ret = new Array(
			time, 
			bat.life + "%",
			source,
			0,
			bat.cycles, 
			health);
			
		if (bat.capacity > 0)
			break;
	}
	
	return ret;
}

iStatNano.isLaptop = function()
{
	if (iStatNano.bat.getBatteryCount() == 0)
		this.battery();
	return (iStatNano.bat.getBatteryCount() > 0);
}

iStatNano.load = function()
{
	iStatNano.cpu.getUsage();
	var ret = new Array(iStatNano.cpu.loadAverage());
	return ret;
}

iStatNano.processinfo = function()
{
	return "";
}

iStatNano.getAppPath = function(app)
{
	return "";
}

iStatNano.getPsName = function(id)
{
	return "";
}

iStatNano.getselfpid = function()
{
	return 0;
}

iStatNano.hasDiskSetupChanged = function()
{
	return false;
}

iStatNano.hasNetworkSetupChanged = function()
{
	return false;
}

iStatNano.installIntelModule = function()
{
	// not implemented
}

iStatNano.wasIntelModuleInstalled = function()
{
	return false;
}

var ll;
Kludget.system = function(cmd, handler)
{	
	var cmdObject = new Command;
	
	if (cmd.indexOf("ps") == 0)
	{
		alert(cmd);
		return cmdObject;
	}
	cmdObject.execute(cmd, handler);
	return cmdObject;
}