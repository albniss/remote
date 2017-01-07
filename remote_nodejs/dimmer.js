function Dimmer() {
	this.count = 0;
	this.messages = {};
	this.busy = 0;
}

Dimmer.prototype.DoWorkCb = function(error, stdout, stderr) {
	//If we still have something to do, do it
	if ( Object.keys(this.messages).length != 0) {
		this.DoWork();
	}
	else {
		this.busy=0;
	}
}

Dimmer.prototype.DoWork = function() {	
	var serial=Object.keys(this.messages)[0];
	var value=this.messages[serial];
	this.count++;
	
	//Now remove the message (since it has already been processed)
	delete this.messages[serial];
	
	//Inform that this object is busy
	this.busy=1;
	
	//Execute command
	var exec = require('child_process').exec;
	
	//Convert value from 0-100 to 0-254
	value = parseInt(value*254/100);
		
	//Execute
	exec('/home/pi/remote/RFSend/Lutron '+this.count+' '+serial+' '+value, this.DoWorkCb.bind(this));
}

Dimmer.prototype.QueueWork = function (serial, value) {
	//if we are currently doing nothing
	if ( this.busy == 0) {
		this.messages[serial]=value;
		this.DoWork();
	}
	//if we are already doing something, we will automagically treat the new value
	else {
		this.messages[serial]=value;
	}
}

module.exports = Dimmer;