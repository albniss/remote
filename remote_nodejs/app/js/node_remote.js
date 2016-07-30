function transmit() {
	var start=performance.now();
	$.ajax({
	  url: "/Ping",
	}).done(function() {
	  alert(performance.now()-start);
	});
}