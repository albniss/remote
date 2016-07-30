// index.js
var port=10001;
var express = require('express');
var app = express();
var http = require('http').Server(app);

app.use(express.static(__dirname+'/app'));
app.use('/bower_components',  express.static(__dirname + '/bower_components'))

app.get('/IRTX', function (req, res) {
	console.log('Remote: '+req.query.remote+" Key: "+req.query.key);
	var exec = require('child_process').exec;
	exec('irsend SEND_ONCE '+req.query.remote+' '+req.query.key, function(error, stdout, stderr){
		if (error === null) {
			console.log("IRTX ok!");
			res.end('{"status":"ok"}');
		}
		else {
			console.log("IRTX error!");
			res.end('{"status":"error"}');
		}
	});
});

app.get('/Yamaha', function (req, res) {
	console.log('Yamaha:'+req.query.payload);
	if (req.query.payload === undefined) {
		res.end('{"status":"error"}');
		return;
	}
	
	var http = require('http');
	var options = {
		host: '192.168.200.250',
		port: '80',
		path: '/YamahaRemoteControl/ctrl',
		method: 'GET'
	};
	var yamaha_req = http.request(options, function(yamaha_res) {
		console.log('Yamaha payload sent! Status code: '+yamaha_res.statusCode);
		if (yamaha_res.statusCode == 200)
			res.end('{"status":"ok"}');
		else
			res.end('{"status":"error"}');	
	});
	yamaha_req.write(req.query.payload);
	yamaha_req.end();
});

http.listen(port, function() {
	console.log('Server listening on port '+port);
});