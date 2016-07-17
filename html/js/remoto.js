var app = angular.module('remoteApp', []);

app.controller('remoteController', function($scope,$http,$q) {

	function doAjaxYamaha(payload, port) {
		console.log("doajaxYamaha:"+command);

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

		url+="/cgi-bin/yamaha.sh?payload="+payload;
		
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

	$scope.Yamaha = function (status,port) {
		switch (status) {
			case "ON":
				break;
			case "OFF":
				break;
			case "VOLUP":
				break;
			case "VOLDN":
				break;
			case "NETRADIO":
				break;
			case "TUNER":
				break;
			case "AIRPLAY":
				break;
			case "HDMI1":
				break;
			case "HDMI2":
				break;
			case "HDMI3":
				break;
			case "HDMI4":
				break;
		}

		doAjaxYamaha('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="PUT"><Main_Zone><Volume><Lvl><Val>Up</Val><Exp></Exp><Unit></Unit></Lvl></Volume></Main_Zone></YAMAHA_AV>',1818);
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