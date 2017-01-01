var app = angular.module('remoteApp', []);

app.controller('remoteController', function($scope,$http,$q) {

	function doAjaxYamaha(payload) {
		console.log("doajaxYamaha:"+payload);

		var deferred = $q.defer();
		
		var url="";

		port = window.location.port;
		if (window.location.hostname == "192.168.200.201" || window.location.hostname == "192.168.200.202") {
			if      (port==1818) {
				url = "http://192.168.200.201:1818";
			}
			else if (port==1819) {
				url = "http://192.168.200.202:1819";
			}
		}
		else {
			url="http://"+window.location.hostname+":"+port;
		}

		url+="/cgi-bin/yamaha.py?payload="+payload;
		
		$http.get(url).then(
			function (response) {
				if (response.data.ok == 1) {
					console.log("doAjaxYamaha: OK");
					deferred.resolve("OK");
				}
				else {
					console.log("Internal error!");
					deferred.reject("Internal error!")
				}
			},
			function () {
				console.log("Connection error!");
				deferred.reject("Connection error!")
			});

		return deferred.promise;
	}

	function doAjax(command, port) {
		console.log("doajax:"+command);

		var deferred = $q.defer();
		
		var url="";
		if (window.location.hostname == "192.168.200.201" || window.location.hostname == "192.168.200.202") {
			if      (port==1818) {
				url = "http://192.168.200.201:1818";
			}
			else if (port==1819) {
				url = "http://192.168.200.202:1819";
			}
		}
		else {
			url="http://"+window.location.hostname+":"+port;
		}

		url+="/cgi-bin/mandar.sh?remote="+command[0]+"&command="+command[1];
		
		$http.get(url).then(
			function (response) {
				if (response.data.ok == 1) {
					console.log("doajax: OK: "+command);
					deferred.resolve("OK");
				}
				else {
					console.log("Internal error!");
					deferred.reject("Internal error!")
				}
			},
			function () {
				console.log("Connection error!");
				deferred.reject("Connection error!")
			});

		return deferred.promise;
	}

	function statusTX () {
		$('#header').css("background-color","#006400");
	}

	function statusError () {
		$('#header').css("background-color","#f00");
	}

	function statusNormal () {
		$('#header').css("background-color","#428bca");
	}

	$scope.Yamaha = function (command) {
		console.log(command);

		switch (command) {
			case "ON":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><System><Power_Control><Power>On</Power></Power_Control></System></YAMAHA_AV>');
				break;
			case "OFF":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><System><Power_Control><Power>Standby</Power></Power_Control></System></YAMAHA_AV>');
				break;
			case "VOLUP":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Volume><Lvl><Val>Up</Val><Exp></Exp><Unit></Unit></Lvl></Volume></Main_Zone></YAMAHA_AV>');
				break;
			case "VOLDN":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Volume><Lvl><Val>Down</Val><Exp></Exp><Unit></Unit></Lvl></Volume></Main_Zone></YAMAHA_AV>');
				break;
			case "NETRADIO":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>NET RADIO</Input_Sel></Input></Main_Zone></YAMAHA_AV>');
				break;
			case "TUNER":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>TUNER</Input_Sel></Input></Main_Zone></YAMAHA_AV>');
				break;
			case "AIRPLAY":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>AirPlay</Input_Sel></Input></Main_Zone></YAMAHA_AV>');
				break;
			case "HDMI1":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>HDMI1</Input_Sel></Input></Main_Zone></YAMAHA_AV>');
				break;
			case "HDMI2":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>HDMI2</Input_Sel></Input></Main_Zone></YAMAHA_AV>');
				break;
			case "HDMI3":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>HDMI3</Input_Sel></Input></Main_Zone></YAMAHA_AV>');
				break;
			case "HDMI4":
				doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Input><Input_Sel>HDMI4</Input_Sel></Input></Main_Zone></YAMAHA_AV>');
				break;
		}
	}

	$scope.Samsung = function (status, port) {
		var command;

		if      (status=='ON')
			command = "CMD_"+$("#ar2 #temp").val()+"_"+$("#ar2 #mode").val()+"_"+$("#ar2 #fan").val()+"_"+$("#ar2 #low_noise").val()+"_"+$("#ar2 #swing").val();
		else if (status=='OFF')
			command = "CMD_OFF";
		else if (status=='HEAT23')
			command = "CMD_23_HEAT_1_1_0";
		else if (status=='COOL23')
			command = "CMD_23_COOL_1_1_0";
		else if (status=='CUSTOM')
			command = "CMD_"+$("#ar2 #temp").val()+"_"+$("#ar2 #mode").val()+"_"+$("#ar2 #fan").val()+"_"+$("#ar2 #low_noise").val()+"_"+$("#ar2 #swing").val();

		console.log(command);

		statusTX();
		doAjax(["samsung",command],port)
		.then(
		function() {
			console.log("Sucesso final");
			statusNormal();
		})
		.catch(function () {
			console.log("Erro final");
			statusError();
		});		
	}

	$scope.Fujitsu = function (status, port) {
		var command;

		if      (status=='ON')
			command = "CMD_"+$("#ar #temp").val()+"_"+$("#ar #mode").val()+"_"+$("#ar #fan").val()+"_"+$("#ar #low_noise").val()+"_1";
		else if (status=='OFF')
			command = "CMD_OFF";
		else if (status=='HEAT23')
			command = "CMD_23_HEAT_1_1_1";
		else if (status=='COOL23')
			command = "CMD_23_COOL_1_1_1";
		else if (status=='CUSTOM')
			command = "CMD_"+$("#ar #temp").val()+"_"+$("#ar #mode").val()+"_"+$("#ar #fan").val()+"_"+$("#ar #low_noise").val()+"_0";

		console.log(command);

		statusTX();
		doAjax(["fujitsu",command],port)
		.then(
		function() {
			console.log("Sucesso final");
			statusNormal();
		})
		.catch(function () {
			console.log("Erro final");
			statusError();
		});		
	}

	$scope.Optoma = function (command, port) {
		statusTX();
		doAjax(["optoma",command],port)
		.then(
		function() {
			console.log("Sucesso final");
			statusNormal();
		})
		.catch(function () {
			console.log("Erro final");
			statusError();
		});
	}	

	$scope.Net = function (command, port) {
		statusTX();
		doAjax(["net",command],port)
		.then(
		function() {
			console.log("Sucesso final");
			statusNormal();
		})
		.catch(function () {
			console.log("Erro final");
			statusError();
		});
	}

    $scope.Atividade = function(tipo, port) {
    	switch (tipo){ 
    		case 'Net':
    			statusTX();
    			doAjax(["optoma","KEY_POWER"],port)
    			.then(function(){return doAjax(["net","KEY_POWER"],port)})
				.then(function(){return doAjax(["optoma","KEY_2"],port)})
    			.then(
    			function() {
    				console.log("Sucesso final");
    				statusNormal();
    			})
    			.catch(function () {
    				console.log("Erro final");
    				statusError();
    			});
	        	break;
	        case 'Chromecast':
	            statusTX();
    			doAjax(["optoma","KEY_POWER"],port)
				.then(function(){return doAjax(["optoma","KEY_1"],port)})
    			.then(
    			function() {
    				console.log("Sucesso final");
    				statusNormal();
    			})
    			.catch(function () {
    				console.log("Erro final");
    				statusError();
    			});
	        	break;
	        	break;
	        default:
	        	alert("Invalid activity!");
    	};
    };

    console.log("Init");

	$(".knob").knob({
		'release' : function (v) {
			console.log("release!");
			//Volume information
			//0-100 on the knob equals -80db to 0db (-800 to 000)
			var db = 10 * parseInt($("#vol_knob").val());
			doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Volume><Lvl><Val>'+db+'</Val><Exp>1</Exp><Unit>dB</Unit></Lvl></Volume></Main_Zone></YAMAHA_AV>');
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
});

function SamValidate() {
	if ($("#ar2 #mode").val() == "AUTO") {
		$("#ar2 #fan").val("AUTO");
		$("#ar2 #fan").prop("disabled",true);
	}
	else {
		$("#ar2 #fan").prop("disabled",false);
	}
}