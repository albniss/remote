// index.js
var express = require('express');
var app = express();
var http = require('http').Server(app);

//Enable CORS
app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});

app.use(express.static(__dirname+'/app'));
app.use('/bower_components',  express.static(__dirname + '/bower_components'))

app.get('/Weather', function (req, res) {
	console.log('Weather');
	
	var fs = require('fs');
	fs.readFile("WEATHER.csv", 'utf8', function(err, contents) {
		var ret=Array();
		var lines=contents.split('\n');
		for (i in lines) {
			var temp=lines[i].split(";");
			if (temp.length == 7)
				ret.push(temp);
		}
		res.end(JSON.stringify(ret));
	});
});

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
		method: 'POST'
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

var port=process.argv[2];
http.listen(port, function() {
	console.log('Server listening on port '+port);
});