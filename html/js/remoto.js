var app = angular.module('remoteApp', []);

app.controller('remoteController', function($scope,$http,$q) {

	function doAjax(command) {
		console.log("doajax:"+command);

		var deferred = $q.defer();

		var url="/cgi-bin/mandar.sh?remote="+command[0]+"&command="+command[1];
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

	$scope.Fujitsu = function (status) {
		var command;

		if      (status=='ON')
			command = "CMD_"+$("#temp").val()+"_"+$("#mode").val()+"_"+$("#fan").val()+"_"+$("#low_noise").val()+"_1";
		else if (status=='OFF')
			command = "CMD_OFF";
		else if (status=='HEAT23')
			command = "CMD_23_HEAT_1_1_1";
		else if (status=='COOL23')
			command = "CMD_23_COOL_1_1_1";
		else if (status=='CUSTOM')
			command = "CMD_"+$("#temp").val()+"_"+$("#mode").val()+"_"+$("#fan").val()+"_"+$("#low_noise").val()+"_0";

		console.log(command);

		statusTX();
		doAjax(["fujitsu",command])
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

	$scope.Optoma = function (command) {
		statusTX();
		doAjax(["optoma",command])
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

	$scope.Net = function (command) {
		statusTX();
		doAjax(["net",command])
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

    $scope.Atividade = function(tipo) {
    	switch (tipo){ 
    		case 'Net':
    			statusTX();
    			doAjax(["optoma","KEY_POWER"])
    			.then(function(){return doAjax(["net","KEY_POWER"])})
				.then(function(){return doAjax(["optoma","KEY_2"])})
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
    			doAjax(["optoma","KEY_POWER"])
				.then(function(){return doAjax(["optoma","KEY_1"])})
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