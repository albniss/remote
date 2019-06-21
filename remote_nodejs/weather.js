function Weather(location) {
	this.location = location;
	this.timer = null;
	
	var fs = require('fs');
	this.mongodbUri = fs.readFileSync(__dirname + '/connectionString.txt', 'utf8');
}

Weather.prototype.Start = function () {
	this.timer = setInterval(this.weatherTimer, 60*1000, this.mongodbUri, this.location);
}

Weather.prototype.Stop = function () {
	if (this.timer != null)
		clearInterval(this.timer);
}

Weather.prototype.weatherTimer = function (uri, location) {
	//Execute command
	var exec = require('child_process').exec;
			
	//Execute
	exec('WL', (err, stdout, stderr) => 
	{
		if (err) {
			console.log("Error running WL");
			return
		}
	
		var params=stdout.split(';');
		var humidity = parseFloat(params[0]);
		var temperature = parseFloat(params[1]);
		
		console.log("Current temperature: " + temperature + " humidity: " + humidity);
				
		const MongoClient = require('mongodb').MongoClient;
		const client = new MongoClient(uri, { useNewUrlParser: true });

		client.connect(err => {
			console.log("Connected!");
		
			if (err) {
				console.log("Error connecting to mongo");
				return;
			}

			const collection = client.db("weather").collection("weather");
		
			collection.insertOne({location: location, temperature: temperature, humidity: humidity}, function(err, res) {
				if (err) {
					console.log("Error inserting into mongo");
					client.close();
					return;
				}
			
				console.log("Document inserted");
				client.close();
			});  
		});
	});	
}

module.exports = Weather;