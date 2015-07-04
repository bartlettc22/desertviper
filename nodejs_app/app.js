var express = require('express');
var app = express();
var server = require('http').Server(app);
var http = require('http');
var path = require('path');
var routes = require('./routes/index');
var bodyParser = require('body-parser');
var io = require('socket.io')(server);
var Enum = require('enum');
var serialport = require("serialport");


/*
 * Serial Communication Setup
 */
var serialCommands = new Enum(['kSetLed', 'kStatus', 'kLog', 'kDrive', 'kCalibrate', 'kTurn']);
var SerialPort = serialport.SerialPort;

// Last time a message was received from Arduino - used to tell if connection was broken
var lastSerialDataTime = new Date().getTime();
// list serial ports:
/*serialport.list(function (err, ports) {
  ports.forEach(function(port) {
    console.log(port.comName);
  });
});*/

if(process.platform === 'win32') {
  var serialPortName = 'COM5';
} else {
  var serialPortName = '/dev/ttyACM0'; // Arduino Mega
  //var serialPortName = '/dev/ttyUSB0'; // Arduino Pro
}
var serialPortBaud = 115200;

var serialPort  = new SerialPort(serialPortName, {
   baudRate: serialPortBaud,
   // look for return and newline at the end of each data packet:
   parser: serialport.parsers.readline("\n")
}, false);

// Establish Serial connection and set up trigger to check periodically and reestablish if necessary
checkSerialConnection();
setInterval(checkSerialConnection, 1000);

serialPort.on('open', function() {
  console.log('Serial Connection Established on Port "'+serialPortName+'" at '+serialPortBaud+' bps');
});

serialPort.on('data', function(data) {
  lastSerialDataTime = new Date().getTime();
  io.emit('serialData', data);
});

serialPort.on('close', function() {
  console.log('Serial Connection Closed');
});

serialPort.on('error', function(data) {
  //console.log('Serial Connection Error');
  //console.log(data);
});

/*
 * Socket I/O Setup
 */

// Set up events for web clients
io.on('connection', function (socket) {
  
  //console.log('Socket Client Connected');
  
  socket.on('disconnect', function(){
    //console.log('Socket Client Disconnected');
  });  

  // Command sent from website, pass on to Arduino
  socket.on('sendCommand', function(data) {
    serialPort.write(data.command+";\n");
    io.emit('commandSent', { command: data.command+";\n" });
  });

  socket.on('toggleSerial', function(data) {
    if(data.state) {
      io.emit('socketFeedback', { message: "toggleSerial: Turning serial communication on" });
      serialPort.open();
    } else {
      io.emit('socketFeedback', { message: "toggleSerial: Turning serial communication off" });
      serialPort.close();
    }
  });

});

/*
 * App Setup
 */
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(express.static(path.join(__dirname, 'public')));
app.use('/bower_components',  express.static(__dirname + '/bower_components'));

app.use('/', routes);

// Catch 404 and forwarding to error handler
app.use(function(req, res, next) {
    var err = new Error('Not Found');
    err.status = 404;
    next(err);
});

// Other error handling
app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
        message: err.message,
        error: err
    });
});

// Start NodeJS Server
server.listen(8081, function() {
  console.log("NodeJS Server Started...");
});

function checkSerialConnection() {

  var hasTimedOut = ((new Date().getTime() - lastSerialDataTime) > 1000);

  if(hasTimedOut) {
  //if(!serialPort.isOpen()) {
    serialPort.open(function(error) {
      if ( error ) {
        console.log('Serial connection not found on "'+serialPortName+'", attempting to reconnect...');
        console.log(error);
      }
    });
  }
}
