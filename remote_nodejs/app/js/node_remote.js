function statusTX () {
	$('#header').css("background-color","#006400");
}

function statusError () {
	$('#header').css("background-color","#f00");
}

function statusNormal () {
	$('#header').css("background-color","#428bca");
}

function getURL(room) {
	var url;
	
	if (window.location.hostname == "192.168.200.201" || window.location.hostname == "192.168.200.202") {
		if ("bedroom"==room) {
			url="http://192.168.200.201:1818";
		}
		else if ("living"==room) {
			url="http://192.168.200.202:1819";
		}
	}
	else {
		if ("bedroom"==room) {
			url="http://"+window.location.hostname+":1818";
		}
		else if ("living"==room) {
			url="http://"+window.location.hostname+":1819";
		}
	}
	
	return url;
}

function _IRTX(remote,key,room) {
	var url=getURL(room);
	var deferred = jQuery.Deferred();
	
	$.ajax({
		url: url+"/IRTX?remote="+remote+"&key="+key,
		success: function(result) {
			if (JSON.parse(result).status=="ok")
				deferred.resolve("ok");
			else
				deferred.reject("Internal error!");	
		},
		error: function(result) {
			deferred.reject("Internal error!");
		}
	});
	
	return deferred;
}

function IRTX(remote, key, room) {
	statusTX();
	_IRTX(remote,key,room).then(
		function() {
			console.log("Sucesso final");
			statusNormal();
		}).catch(
		function () {
			console.log("Erro final");
			statusError();
		});
}

function Fujitsu(onoff,temp,fan,mode,lownoise,room) {
	var command;
	
	if ("ON" == onoff) {
		command="CMD_"+temp+"_"+mode+"_"+fan+"_"+lownoise+"_1";
	}
	else if ("OFF" == onoff) {
		command="CMD_OFF";
	}
	else {
		command="CMD_"+temp+"_"+mode+"_"+fan+"_"+lownoise+"_0";
	}
	IRTX("fujitsu",command,room);
}

function Samsung(onoff,temp,fan,mode,lownoise,swing,room) {
	var command;
	
	if ("ON" == onoff) {
		command = "CMD_"+temp+"_"+mode+"_"+fan+"_"+lownoise+"_"+swing;
	}
	else {
		command="CMD_OFF";
	}				
	IRTX("samsung",command,room);
}

function _Yamaha(payload) {
	var url=getURL('living');
	var deferred = jQuery.Deferred();
	
	$.ajax({
		url: url+"/Yamaha?payload="+payload,
		success: function(result) {
			console.log("Yamaha ok");
			deferred.resolve("ok");
		},
		error: function() {
			console.log("Yamaha error");
			deferred.reject("Internal error!");
		}
	});	
	
	return deferred;
}

function Yamaha(command) {
	var payload;
	
	statusTX();
	
	switch (command) {
		case "ON":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><System><Power_Control><Power>On</Power></Power_Control></System></YAMAHA_AV>';
			break;
		case "OFF":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><System><Power_Control><Power>Standby</Power></Power_Control></System></YAMAHA_AV>';
			break;
		case "VOLUP":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Volume><Lvl><Val>Up</Val><Exp></Exp><Unit></Unit></Lvl></Volume></Main_Zone></YAMAHA_AV>';
			break;
		case "VOLDN":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Volume><Lvl><Val>Down</Val><Exp></Exp><Unit></Unit></Lvl></Volume></Main_Zone></YAMAHA_AV>';
			break;
		case "NETRADIO":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>NET RADIO</Input_Sel></Input></Main_Zone></YAMAHA_AV>';
			break;
		case "TUNER":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>TUNER</Input_Sel></Input></Main_Zone></YAMAHA_AV>';
			break;
		case "AIRPLAY":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>AirPlay</Input_Sel></Input></Main_Zone></YAMAHA_AV>';
			break;
		case "HDMI1":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>HDMI1</Input_Sel></Input></Main_Zone></YAMAHA_AV>';
			break;
		case "HDMI2":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>HDMI2</Input_Sel></Input></Main_Zone></YAMAHA_AV>';
			break;
		case "HDMI3":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>HDMI3</Input_Sel></Input></Main_Zone></YAMAHA_AV>';
			break;
		case "HDMI4":
			payload='<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>HDMI4</Input_Sel></Input></Main_Zone></YAMAHA_AV>';
			break;
		default:
			payload=command;
			break;
	}
	
	_Yamaha(payload).then(
		function() {
			console.log("Sucesso final");
			statusNormal();
		}).catch(
		function () {
			console.log("Erro final");
			statusError();
		});
}

function SamValidate() {
	if ($("#ar2 #mode").val() == "AUTO") {
		$("#ar2 #fan").val("AUTO");
		$("#ar2 #fan").prop("disabled",true);
	}
	else {
		$("#ar2 #fan").prop("disabled",false);
	}
}

console.log("Init");

$(".knob").knob({
	'release' : function (v) {
		console.log("release!");
		//Volume information
		//0-100 on the knob equals -80db to 0db (-800 to 000)
		var db = 10 * parseInt($("#vol_knob").val());
		Yamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Volume><Lvl><Val>'+db+'</Val><Exp>1</Exp><Unit>dB</Unit></Lvl></Volume></Main_Zone></YAMAHA_AV>');
	}
});

var app = document.getElementById("app");

Hammer(app).on("swiperight", function() {
var $tab = $('.level1.active .nav-pills .active').prev();
if ($tab.length > 0)
  $tab.find('a').tab('show');
});

Hammer(app).on("swipeleft", function() {
var $tab = $('.level1.active .nav-pills .active').next();
if ($tab.length > 0)
  $tab.find('a').tab('show');
});