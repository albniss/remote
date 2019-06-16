// index.js
var compression = require('compression');
var express = require('express');
var app = express();
app.use(compression());
var http = require('http').Server(app);
var io = require('socket.io');
var Dimmer = require('./dimmer.js');
var dimmer = new Dimmer();

//Enable CORS
app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});

app.use(express.static(__dirname+'/app', {maxAge: '365d'}));
app.use('/bower_components',  express.static(__dirname + '/bower_components', {maxAge: '365d'}));
app.use('/node_modules',  express.static(__dirname + '/node_modules', {maxAge: '365d'}));

app.get('/Weather', function (req, res) {
	console.log('Weather');
	
	var fs = require('fs');
	fs.readFile("/home/pi/WEATHER.csv", 'utf8', function(err, contents) {
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

var listener=io.listen(http);
var luz_jantar=0;
var luz_sala=0;
var luz_baby=0;

listener.sockets.on('connection',function(socket){
	console.log('Got it!');
	var data={};
	
	//Living
	if (port==1819)
	{
		console.log('Got: Living');
		data.value=luz_jantar;
		socket.emit('luz_jantar',data);
		data.value=luz_sala;
		socket.emit('luz_sala',data);
		
		var http = require('http');
		var options = {
			host: '192.168.200.250',
			port: '80',
			path: '/YamahaRemoteControl/ctrl',
			method: 'POST'
		};
		var yamaha_req = http.request(options, function(response) {
			var completeResponse = '';
			response.on('data', function (chunk) {
				completeResponse += chunk;
			});
			response.on('end', function() {
				var found = completeResponse.match(/<Val>(.*)<\/Val>/);
				console.log(found[1]);
				var data={};
				data.value=parseInt((found[1]-1)/10);
				socket.emit('yamaha_vol',data);
			});
		});
		yamaha_req.write('<?xml version="1.0" encoding="utf-8"?><YAMAHA_AV cmd="GET"><Main_Zone><Volume><Lvl>GetParam</Lvl></Volume></Main_Zone></YAMAHA_AV>');
		yamaha_req.end();		
		
		socket.on('luz_jantar', function(data){
			luz_jantar=data.value;
			dimmer.QueueWork("6e4c1184",data.value);
			socket.broadcast.emit('luz_jantar',data);
		});
	
		socket.on('luz_sala', function(data){
			luz_sala=data.value;
			dimmer.QueueWork("6e4c1344",data.value);
			socket.broadcast.emit('luz_sala',data);
		});
	}
	
	//Bedroom
	else if (port==1818)
	{
		console.log('Got: Bedroom');
		
		data.value=luz_baby;
		socket.emit('luz_baby',data);
		
		socket.on('luz_baby', function(data){
			luz_baby=data.value;
			dimmer.QueueWork("68d252b4",data.value);
			socket.broadcast.emit('luz_baby',data);
		});
	}
});
