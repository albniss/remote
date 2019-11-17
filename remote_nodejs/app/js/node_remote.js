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
	
	if (window.location.hostname == "192.168.200.201" || 
		window.location.hostname == "192.168.200.202" || 
		window.location.hostname == "192.168.200.203" ) {
		if ("bedroom"==room) {
			url="http://192.168.200.201:1818";
		}
		else if ("living"==room) {
			url="http://192.168.200.202:1819";
		}
		else if ("baby"==room) {
			url="http://192.168.200.203:1820";
		}
	}
	else {
		if ("bedroom"==room) {
			url="http://"+window.location.hostname+":1818";
		}
		else if ("living"==room) {
			url="http://"+window.location.hostname+":1819";
		}
		else if ("baby"==room) {
			url="http://"+window.location.hostname+":1820";
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

function Daikin(onoff,temp,fan,mode,eco,swing,room) {
	var command;
	
	if ("ON" == onoff) {
		command = "CMD_"+temp+"_"+mode+"_"+fan+"_"+eco+"_"+swing;
	}
	else {
		command="CMD_OFF";
	}				
	IRTX("daikin",command,room);
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
	if ($("#sala-ar #sala-mode").val() == "AUTO") {
		$("#sala-ar #sala-fan").val("AUTO");
		$("#sala-ar #sala-fan").prop("disabled",true);
	}
	else {
		$("#sala-ar #sala-fan").prop("disabled",false);
	}
}

function heatMapColorforValue(value){
  var h = Math.floor((1.0 - value) * 240);
  return "hsl(" + h + ", 100%, 50%)";
}

function GetWeather() {
	console.log("GetWeather");
	
	$.ajax({
		url: "/weather",
		success: function(result) {
			result=JSON.parse(result);
			
			var now=new Date().getTime();
			
			var colors=Array();		
			var data=Array();
			for (i in result) {		
				var time=new Date(result[i][2],result[i][3]-1,result[i][4],result[i][5],result[i][6]).getTime();
				var deltah = (now-time)/1000/60/60;

				//We only want data that is at most 1-day old
				if (deltah <= 24) {					
					var temp=Array();
					temp.x=result[i][1];
					temp.y=result[i][0];
					data.push(temp);
					colors.push(heatMapColorforValue((deltah/24)));
				}
			}
			scatterChart.data.datasets[0].data = data;
			scatterChart.data.datasets[0].pointBackgroundColor = colors;
			scatterChart.update();
		},
		error: function() {
			console.log("Weather error");
		}
	});
}

console.log("Init");

var socket_living = io.connect(getURL('living'));
var socket_baby = io.connect(getURL('bedroom'));

///Volume stuff
var last_vol=-1000;
socket_living.on('yamaha_vol', function(data){
	if (last_vol != data.value) {
		last_vol = data.value;
		$("#vol_knob").val(data.value).trigger('change');
	}
});
$("#vol_knob").knob({
	'release' : function (v) {
		if (parseInt($("#vol_knob").val()) != last_vol)
		{
			last_vol=parseInt($("#vol_knob").val());
			//Volume information
			//0-100 on the knob equals -80db to 0db (-800 to 000)
			var db = 10 * parseInt($("#vol_knob").val());
			Yamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Volume><Lvl><Val>'+db+'</Val><Exp>1</Exp><Unit>dB</Unit></Lvl></Volume></Main_Zone></YAMAHA_AV>');
		}
	}
});

///Lighting - Living room
socket_living.on('luz_sala', function(data){
    $("#sala_knob").val(data.value).trigger('change');
});
$("#sala_knob").knob({
	'change' : function (v) {
		var i=parseInt(v+0.5);
		socket_living.emit('luz_sala',{'value':i});
	}
});

///Lighting - Dining room
socket_living.on('luz_jantar', function(data){
    $("#jantar_knob").val(data.value).trigger('change');
});
$("#jantar_knob").knob({
	'change' : function (v) {
		var i=parseInt(v+0.5);
		socket_living.emit('luz_jantar',{'value':i});
	}
});

///Lighting - Baby room
socket_baby.on('luz_baby', function(data){
    $("#baby_knob").val(data.value).trigger('change');
});
$("#baby_knob").knob({
	'change' : function (v) {
		var i=parseInt(v+0.5);
		socket_baby.emit('luz_baby',{'value':i});
	}
});

///Lighting - Bedroom
socket_baby.on('luz_casal', function(data){
    $("#casal_knob").val(data.value).trigger('change');
});
$("#casal_knob").knob({
	'change' : function (v) {
		var i=parseInt(v+0.5);
		socket_baby.emit('luz_casal',{'value':i});
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
