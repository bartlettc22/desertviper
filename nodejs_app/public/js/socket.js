'use strict';

// Set up socket connection and attach event handlers
var socket = io.connect('http://'+window.location.host);

var myChart;

// Compass Globals
var compassCanvas;
var compassContext;
var compassImage;
var needleImage;

// Charting Globals

var chartCurrent = {
  chart: "",
  dataFront: [],
  dataRear: [],
  xval: 0
};

var chartFrequency = {
  chart: "",
  data: [],
  xval: 0
};

var chartSpeed = {
  chart: "",
  data: [],
  xval: 0
};

var timestamp = ((new Date()).getTime() / 1000)|0;
var updateChart;
var count
var keys_down = {};

var serialCommands = { 'kSetLed': 0, 'kStatus': 1, 'kLog': 2, 'kDrive': 3, 'kCalibrate': 4, 'kTurn': 5, 'kSetConfig': 6 }
var configOptions = {
  'kLogSpeed': 0,
  'kSampleSpeed': 1
}
var TURN_DIRECTION_BRAKE = 0;
var TURN_DIRECTION_LEFT = 1;
var TURN_DIRECTION_RIGHT = 2;
var MOTOR_FRONT = 0;
var MOTOR_REAR = 1;
var MOTOR_BOTH = 2;
var MOTOR_STEER = 3;


var MOTOR_DIRECTION_CW = 0; // Forward/Right
var DRIVE_DIRECTION_FORWARD = 0; // Forward/Right
var TURN_DIRECTION_RIGHT = 0; // Forward/Right
var MOTOR_DIRECTION_CCW = 1;// Reverse/Left
var DRIVE_DIRECTION_REVERSE = 1;// Reverse/Left
var TURN_DIRECTION_LEFT = 1; // Reverse/Left

var MOTOR_DIRECTION_BRAKE_LOW = 3; // Brake LOW
var DRIVE_DIRECTION_BRAKE_LOW = 3; // Brake LOW
var TURN_DIRECTION_BRAKE = 3; // Brake LOW

var maxLogs = 600;
var logCounter = 0;
var isPaused = false;
var lastSerialDataTime = new Date().getTime();
setInterval(checkSerialConnection, 2000);

socket.on('reconnect', function (error) {
	console.log("SocketIO: Reconnected");
	$('#socket-disconnected').hide();
	$('#socket-connected').show();
});

socket.on('connect_error', function (error) {
	console.log("SocketIO: Connection Error");
	$('#socket-connected').hide();
	$('#socket-disconnected').show();
});

// This event is sent by NodeJS as a response to received command
socket.on('socketFeedback', function (data) {
	if(!isPaused) {
		$('#sockin').prepend(data.message.replace(/(?:\;)/g, '').replace(/(?:\r\n|\r|\n)/g, '<br />')); 
	}
});

socket.on('serialData', function (data) {
	
	lastSerialDataTime = new Date().getTime();

	if(!isPaused) {
		//  The first item is the command and the second is the data/arguments
		var dataParts = data.split(",");

		// If log file, output to log output, otherwise output to command output
		if(dataParts[0] == serialCommands['kLog']) {
			dataParts[1] = dataParts[1].replace(/(?:\;)/g, '');
			$('#socklog').prepend($('<span>'+dataParts[1]+'</span>'));
			if(logCounter >= maxLogs) {
				$('#socklog span:last-child').remove();
			}
			var logParts = dataParts[1].split(":");

			// Set values on page
			$('#footer-runtime').html((logParts[0]/1000).toFixed(2) + ' sec');
			$('#footer-frequency').html(logParts[1] + ' Hz');
			$('#dashboard-frequency').html(logParts[1]);
      		updateChart(chartFrequency, Number(logParts[1]));
			$('#console-steering-pot').html(logParts[2]);
			$('#remote-steering-pot').html(logParts[2]);
			$('#sensors-ticks').html(logParts[3]);
			$('#sensors-speed').html(logParts[8] + ' cm/s');
			$('#sensors-distance').html(logParts[9] + ' cm');
			$('#console-front-motor-current').html(logParts[5] + ' mA');
			$('#dashboard-current-front').html(logParts[5]);
			updateCurrentChart(chartCurrent,Number(logParts[5]),Number(logParts[7]));
			$('#console-rear-motor-current').html(logParts[7] + ' mA');
			$('#dashboard-current-rear').html(logParts[7]);
			$('#dashboard-speed-value').html(logParts[8] + ' cm/s');
			updateChart(chartSpeed, Number(logParts[8]));
			$('#footer-speed').html(logParts[8] + ' cm/s');
			$('#dashboard-speed').html(logParts[8]);

			if(logParts[12] == "1") {
				$('#footer-front-motor-fault-healthy').hide();
				$('#footer-front-motor-fault').show();
			} else {
				$('#footer-front-motor-fault').hide();
				$('#footer-front-motor-fault-healthy').show();
			}

			if(logParts[13] == "1") {
				$('#footer-rear-motor-fault-healthy').hide();
				$('#footer-rear-motor-fault').show();
			} else {
				$('#footer-rear-motor-fault').hide();
				$('#footer-rear-motor-fault-healthy').show();
			}
			
			$('#console-rf-a').html(logParts[14]);
			toggleRF('a', logParts[14]);
			$('#console-rf-b').html(logParts[15]);
			toggleRF('b', logParts[15]);
			$('#console-rf-c').html(logParts[24]);
			toggleRF('c', logParts[24]);
			$('#console-rf-d').html(logParts[25]);
			toggleRF('d', logParts[25]);

			$('#console-mag-x').html(logParts[26]);
			$('#console-mag-y').html(logParts[27]);
			$('#console-mag-z').html(logParts[28]);
			$('#sensors-acc-x').html(logParts[30]);
			$('#sensors-acc-y').html(logParts[31]);
			$('#sensors-acc-z').html(logParts[32]);
			$('#sensors-gyro-x').html(logParts[33]);
			$('#sensors-gyro-y').html(logParts[34]);
			$('#sensors-gyro-z').html(logParts[35]);

			$('#sensors-pressure').html(logParts[40] + ' mb');
			$('#sensors-pressure-hg').html(logParts[41] + ' inHG');
			$('#sensors-pressure0').html(logParts[42] + ' mb');
			$('#sensors-pressure0-hg').html(logParts[43] + ' inHG');
			$('#sensors-temperature').html(logParts[38] + '&deg;C');
			$('#sensors-temperature-F').html(logParts[39] + '&deg;F');	
			$('#sensors-altitude-change').html(logParts[36] + ' m');	
			$('#sensors-pressure-baseline').html(logParts[37] + ' mb');

			$('#console-steering-pot-left-max').html(logParts[16]);
			//configScope.$apply(function () { configScope.config.steeringLeftMax = logParts[16]; });
			$('#console-steering-pot-right-max').html(logParts[17]);
			//configScope.$apply(function () { configScope.config.steeringLeftMax = logParts[17]; });
			$('#console-steering-pot-center').html(logParts[18]);
			$('#footer-range').html(logParts[19] + ' cm');		
			$('#sensors-range-duration').html(logParts[20] + ' &micro;s');
			$('#sensors-range-distance').html(logParts[19] + ' cm');
			$('#sensors-range-timeout').html(logParts[29] + ' &micro;s');

			$('#remote-front-current-speed').html(Math.round(logParts[22]*100, 0) + '%');
			//$('#remote-front-goal-speed').html(Math.round(logParts[4]*100, 0) + '%');
			$('#remote-rear-current-speed').html(Math.round(logParts[23]*100, 0) + '%');
			//$('#remote-rear-goal-speed').html(logParts[6]);

			$('#remote-steering-left-goal').html(logParts[44]);
			$('#remote-steering-right-goal').html(logParts[45]);

			var card_dir = "";
			if(logParts[21] <= 22.5 || logParts[21] >= 337.5) {
				card_dir = 'N';
			} else if(logParts[21] <= 67.5) {
				card_dir = 'NE';
			} else if(logParts[21] < 112.5) {
				card_dir = 'E';
			} else if(logParts[21] <= 157.5) {
				card_dir = 'SE';
			} else if(logParts[21] <= 202.5) {
				card_dir = 'S';
			} else if(logParts[21] <= 247.5) {
				card_dir = 'SW';
			} else if(logParts[21] <= 292.5) {
				card_dir = 'W';
			} else {
				card_dir = 'NW';
			}				

			$('#footer-heading').html(logParts[21] + '&deg; ' + card_dir);
			$('#sensors-heading').html(logParts[21] + '&deg; ' + card_dir);
			rotateCompass(-1*logParts[21]);
			
			logCounter++;
		} else {
			$('#sockin').prepend(dataParts[1].replace(/(?:\;)/g, '').replace(/(?:\r\n|\r|\n)/g, '<br />'));
		}
	}
});

socket.on('commandSent', function (data) {
	if(!isPaused) {
		$('#sockout').prepend(data.command.replace(/(?:\;)/g, '').replace(/(?:\r\n|\r|\n)/g, '<br />'));
	}
});

/*
 * Sets up command events and attaches them to buttons/kepress
 */
$(function() {
	/*$('#toggle-serial').on('switchChange.bootstrapSwitch', function(event, state) {
		console.log("Toggle Serial");
		socket.emit('toggleSerial', { state: state });
	});*/

	/*$('#commandToggleSerial').click(function() {
		socket.emit('sendCommand', { command: $('#serialCommand').val() });
	});*/
	$('#commandCalibrate').click(function() {
		socket.emit('sendCommand', { command: serialCommands['kCalibrate'] });
	});	
	
	$('#serialSubmit').click(function() {
		//console.log($('#serialCommand').val());
		socket.emit('sendCommand', { command: $('#serialCommand').val() });
		$('#serialCommand').val("");
	});

  $('#control-key-up').mousedown(function() {
    drive(DRIVE_DIRECTION_FORWARD);
  }).mouseup(function() {
    drive(DRIVE_DIRECTION_BRAKE_LOW);
  });
  $('#control-key-down').mousedown(function() {
    drive(DRIVE_DIRECTION_REVERSE);
  }).mouseup(function() {
    drive(DRIVE_DIRECTION_BRAKE_LOW);
  }); 
  $('#control-key-left').mousedown(function() {
    turn(TURN_DIRECTION_LEFT, 1);
  }).mouseup(function() {
    turn(TURN_DIRECTION_BRAKE, 1);
  });  
  $('#control-key-right').mousedown(function() {
    turn(TURN_DIRECTION_RIGHT, 1);
  }).mouseup(function() {
    turn(TURN_DIRECTION_BRAKE, 1);
  });    

  //$('#config-log-speed').blur(function() { setConfig('kLogSpeed', $(this).val()); });
  //$('#config-sample-speed').blur(function() { setConfig('kSampleSpeed', $(this).val()); });
	
	// Bind Keyboard Commands
	$(document).keydown(function(e) {
	    if(keys_down[e.which] == null) {
		    switch(e.which) {
		        
            case 32: // space
              isPaused = !isPaused;
            break;  

            case 37: // left
		        	turn(TURN_DIRECTION_LEFT);
		        	$("#control-key-left").addClass("active");
		        break;

		        case 38: // up
		        	drive(DRIVE_DIRECTION_FORWARD);
		        	$("#control-key-up").addClass("active");
		        break;

		        case 39: // right
		        	turn(TURN_DIRECTION_RIGHT);
		        	$("#control-key-right").addClass("active");
		        break;

		        case 40: // down
		        	drive(DRIVE_DIRECTION_REVERSE);
		        	$("#control-key-down").addClass("active");
		        break;

  		    	case 67: // c
  					toggleConsole();	    		
  		    	break;

  		    	case 68: // d
  					toggleDashboard();	    	
  		    	break;

  		    	case 82: // r
  					toggleRemote();	    	
  		    	break;	

  		    	case 83: // s
  					toggleSensors();	    	
  		    	break;
              	

		        default: return; // exit this handler for other keys
		    }
		    keys_down[e.which] = true;
		}

	    e.preventDefault(); // prevent the default action (scroll / move caret)
	});

	// Bind Keyboard Commands
	$(document).keyup(function(e) {
	    switch(e.which) {
	        case 37: // left
	        	turn(TURN_DIRECTION_BRAKE);
	        	$("#control-key-left").removeClass("active");
	        break;

	        case 38: // up
	        	drive(DRIVE_DIRECTION_BRAKE_LOW);
	        	$("#control-key-up").removeClass("active");
	        	keys_down['38'] = true;	        	
	        break;

	        case 39: // right
	        	turn(TURN_DIRECTION_BRAKE);
	        	$("#control-key-right").removeClass("active");
	        break;

	        case 40: // down
	        	drive(DRIVE_DIRECTION_BRAKE_LOW);
	        	$("#control-key-down").removeClass("active");
	        break;

          case 32: // space
  	    	case 67: // c
  	    	case 68: // d
  	    	case 82: // r
  	    	case 83: // r
  	    	break;	        

	        default: return; // exit this handler for other keys
	    }
	    keys_down[e.which] = null;
	    e.preventDefault(); // prevent the default action (scroll / move caret)
	});

});

function drive(direction) {
  //var motor = eval($('input[name=motorSelect]:checked').attr('id'));
  var motor = configScope.settings.motorSelect;
  if(motor == MOTOR_FRONT || motor == MOTOR_BOTH) {
  	//var speed = Number($('#motor-front-speed-slider').val())/100;
  	var speed = configScope.settings.frontMotorGoal/100.0;
  	var command = serialCommands['kDrive'] + ',' + MOTOR_FRONT + ',' + direction + ',' + speed;
  	socket.emit('sendCommand', { command: command });  	
  }
  if(motor == MOTOR_REAR || motor == MOTOR_BOTH) {
  	var speed = configScope.settings.rearMotorGoal/100.0;
  	var command = serialCommands['kDrive'] + ',' + MOTOR_REAR + ',' + direction + ',' + speed;
  	console.log(command);
  	socket.emit('sendCommand', { command: command }); 
  }
}

function turn(direction) {
	var speed = configScope.settings.steeringMotorGoal/100.0;
	if(direction == TURN_DIRECTION_RIGHT) {
		var amount = configScope.settings.steeringAmountRight/100.0;
	} else if (direction == TURN_DIRECTION_LEFT) {
		var amount = configScope.settings.steeringAmountLeft/100.0;
	} else {
		var amount = 1;
	}
	var command = serialCommands['kTurn'] + ',' + direction + ',' + speed + ',' + amount;
	socket.emit('sendCommand', { command: command });
}

function checkSerialConnection() {
  var hasTimedOut = ((new Date().getTime() - lastSerialDataTime) > 1000);
  if(hasTimedOut) {
    $('#serial-connected').hide();
    $('#serial-disconnected').show();
  } else {
    $('#serial-disconnected').hide();
    $('#serial-connected').show();
  }
}



function rotateCompass(degrees){
  compassContext.clearRect(0,0,compassCanvas.width,compassCanvas.height);
  compassContext.save();
  compassContext.translate(compassCanvas.width/2,compassCanvas.height/2);

  compassContext.rotate(degrees*Math.PI/180);
  compassContext.drawImage(compassImage,-compassImage.width/2,-compassImage.width/2);
  compassContext.rotate(-degrees*Math.PI/180);
  compassContext.drawImage(needleImage,-needleImage.width/2,-needleImage.height/2-10);
  compassContext.restore();
}

function updateChart(chart, dataPoint) {
  chart.data.push({
    x: chart.xval,
    y: dataPoint
  });
  chart.xval++;
  if (chart.data.length > chartDataLength) {
    chart.data.shift();        
  }
  chart.chart.render();   
};

function updateCurrentChart(chart, dataPointFront, dataPointRear) {
  chart.dataFront.push({
    x: chart.xval,
    y: dataPointFront
  });
  chart.dataRear.push({
    x: chart.xval,
    y: dataPointRear
  });  
  chart.xval++;
  if (chart.dataFront.length > chartDataLength) {
    chart.dataFront.shift();   
    chart.dataRear.shift();     
  }
  chart.chart.render();   
};

function setConfig(key, value) {
  var command = serialCommands['kSetConfig'] + ',' + key + ',' + value
  socket.emit('sendCommand', { command: command });
}

function toggleRF(id, state) {
  if(state == 1) {
    $('#dashboard-rf-'+id).removeClass('rf-off');
    $('#dashboard-rf-'+id).addClass('rf-on');
  } else {
    $('#dashboard-rf-'+id).removeClass('rf-on');
    $('#dashboard-rf-'+id).addClass('rf-off');
  }
}
