var socket = io.connect('http://'+window.location.host);
var serialCommands = { 'kSetLed': 0, 'kStatus': 1, 'kLog': 2, 'kDrive': 3, 'kCalibrate': 4, 'kTurn': 5 }
var TURN_DIRECTION_BRAKE = 0;
var TURN_DIRECTION_LEFT = 1;
var TURN_DIRECTION_RIGHT = 2;
var DRIVE_MOTOR_FRONT = 0;
var DRIVE_MOTOR_REAR = 1;
var DRIVE_MOTOR_BOTH = 2;
var DRIVE_DIRECTION_FORWARD  =   0;
var DRIVE_DIRECTION_REVERSE  =   1;
var DRIVE_DIRECTION_COAST    =   2;
var DRIVE_DIRECTION_BRAKE_LOW =  3;

// This event is sent by NodeJS as a response to received command
socket.on('socketFeedback', function (data) {
	$('#sockin').prepend(data.message+"\n"); 
});

socket.on('serialData', function (data) {
	
	//  The first item is the command and the second is the data/arguments
	var dataParts = data.split(",");

	// If log file, output to log output, otherwise output to command output
	if(dataParts[0] == serialCommands['kLog']) {
		//$('#socklog').prepend(dataParts[1]);
		var logParts = dataParts[1].split(":");

		// Set values on page
		$('#footer-runtime').html((logParts[0]/1000).toFixed(2) + ' sec');
		$('#footer-frequency').html(logParts[1] + ' Hz');
		$('#console-steering-pot').html(logParts[2]);
		$('#console-ticks').html(logParts[3]);
		$('#console-front-motor-current').html(logParts[5] + ' mA');
		$('#console-rear-motor-current').html(logParts[7] + ' mA');

		$('#console-steering-pot-left-max').html(logParts[16]);
		$('#console-steering-pot-right-max').html(logParts[17]);
		$('#console-steering-pot-center').html(logParts[18]);
		

	} else {
		$('#sockin').prepend(data); 
	}
/*	var entry = [];
	
	var parts2 = parts[1].split(":");
	
	//entry.push({ time: timestamp, y: Number(parts2[1]) });
	//var timestamp = ((new Date()).getTime() / 1000)|0;
	//myChart.push(entry);
	//timestamp++
	updateChart(count, Number(parts2[1]));
	$('#socklog').prepend(parts2);
	//socket.emit('my other event', { my: 'data' });

*/
});

socket.on('commandSent', function (data) {
	$('#sockout').prepend(data.command);
});



/*
 * Sets up command events and attaches them to buttons/kepress
 */
$(function() {
	$('#toggle-serial').on('switchChange.bootstrapSwitch', function(event, state) {
		console.log("Toggle Serial");
		socket.emit('toggleSerial', { state: state });
	});

	$('#commandToggleSerial').click(function() {
		socket.emit('sendCommand', { command: $('#serialCommand').val() });
	});
	$('#commandCalibrate').click(function() {
		socket.emit('sendCommand', { command: serialCommands['kCalibrate'] });
	});	
	$('#commandDriveForward').mousedown(function() {
		//var command = { command: 'kDrive', args: ['DRIVE_MOTOR_FRONT', 'DRIVE_DIRECTION_FORWARD', 1] };
		//socket.emit('sendCommand', command);
		//console.log(command);
		drive();
	});
	$('#commandDriveForward').mouseup(function() {
		//var command = { command: 'kDrive' }; //, args: ['DRIVE_MOTOR_FRONT', 'DRIVE_DIRECTION_BRAKE_LOW', 1] 
		//socket.emit('sendCommand', { command: 'kDrive' });
		//console.log(command);
		stop();
	});	
	$('#serialSubmit').click(function() {
		console.log($('#serialCommand').val());
		socket.emit('sendCommand', { command: $('#serialCommand').val() });
	});
	
	// Bind Keyboard Commands
	$(document).keydown(function(e) {
	    switch(e.which) {
	        case 37: // left
	        	turn(TURN_DIRECTION_LEFT, 1, 1);
	        break;

	        case 38: // up
	        	drive(DRIVE_DIRECTION_FORWARD);
	        break;

	        case 39: // right
	        	turn(TURN_DIRECTION_RIGHT, 1, 1);
	        break;

	        case 40: // down
	        	drive(DRIVE_DIRECTION_REVERSE);
	        break;
	    	case 67: // c
				toggleConsole();	    		
	    	break;
	    	case 68: // d
				toggleDashboard();	    	
	    	break;

	        default: return; // exit this handler for other keys
	    }
	    e.preventDefault(); // prevent the default action (scroll / move caret)
	});

	// Bind Keyboard Commands
	$(document).keyup(function(e) {
	    switch(e.which) {
	        case 37: // left
	        	turn(TURN_DIRECTION_BRAKE, 1, 1);
	        break;

	        case 38: // up
	        	drive(DRIVE_DIRECTION_BRAKE_LOW);
	        break;

	        case 39: // right
	        	turn(TURN_DIRECTION_BRAKE, 1, 1);
	        break;

	        case 40: // down
	        	drive(DRIVE_DIRECTION_BRAKE_LOW);
	        break;

	        default: return; // exit this handler for other keys
	    }
	    e.preventDefault(); // prevent the default action (scroll / move caret)
	});

});

function drive(direction) {
	 var motor = eval($('input[name=motorSelect]:checked').attr('id'));
	 var speed = $('#motor-speed-value').val()/100;
	 var command = serialCommands['kDrive'] + ',' + motor + ',' + direction + ',' + speed;
	socket.emit('sendCommand', { command: command });
}

function turn(direction, speed, amount) {
	var command = serialCommands['kTurn'] + ',' + direction + ',' + speed + ',' + amount;
	socket.emit('sendCommand', { command: command });
}
