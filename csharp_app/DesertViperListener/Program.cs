using System;
using System.Threading;
using TCPServer;
using CommandMessenger;
using CommandMessenger.Transport.Serial;
using System.Text.RegularExpressions;
using Newtonsoft.Json.Linq;
using System.IO;

namespace DesertViperListener
{
	// This is the list of recognized commands. These can be commands that can either be sent or received. 
	// In order to receive, attach a callback function to these events
	// 
	// Default commands
	// Note that commands work both directions:
	// - All commands can be sent
	// - Commands that have callbacks attached can be received
	// 
	// This means that both sides should have an identical command list:
	// one side can either send it or receive it (sometimes both)

	// Commands
	enum Command
	{
		SetLed, // Command to request led to be set in specific state
		Status,
		Log
	};

	public class Program
	{
		public bool RunLoop { get; set; }
		private SerialTransport _serialTransport;
		private CmdMessenger _cmdMessenger;
		private bool _ledState;
		private Server _tcpListener;
		private string homePath;
		//private StreamWriter LogWriter;

		// Setup function
		public void Setup()
		{

			homePath = (Environment.OSVersion.Platform == PlatformID.Unix || 
				Environment.OSVersion.Platform == PlatformID.MacOSX)
				? Environment.GetEnvironmentVariable("HOME")
				: Environment.ExpandEnvironmentVariables("%HOMEDRIVE%%HOMEPATH%");
			Logger.Log ("Logging to: " + homePath);

			//StreamWriter LogWriter = File.AppendText(homePath+"/log.txt");

			// Create Serial Port object
			_serialTransport = new SerialTransport();
			// Set com port
			if (Program.IsLinux) {
				_serialTransport.CurrentSerialSettings.PortName = "/dev/ttyUSB0";
			} else {
				_serialTransport.CurrentSerialSettings.PortName = "COM5";    
			}
			_serialTransport.CurrentSerialSettings.BaudRate = 115200;     // Set baud rate
			_serialTransport.CurrentSerialSettings.DtrEnable = false;     // For some boards (e.g. Sparkfun Pro Micro) DtrEnable may need to be true.

			Init ();

			// Create TCP Listener
			_tcpListener = new Server();

			// Create a class that listens to the TCP receive event.
			CommandListener listener = new CommandListener(_tcpListener, _cmdMessenger);
		}

		public void Init() {
			// Initialize the command messenger with the Serial Port transport layer
			_cmdMessenger = new CmdMessenger(_serialTransport, BoardType.Bit16);

			// Attach the callbacks to the Command Messenger
			AttachCommandCallBacks();

			// Start listening
			_cmdMessenger.Connect(); 
		}

		// Loop function
		public void Loop()
		{
			if (!_serialTransport.IsConnected ()) {
				Logger.Log ("Arduino not connected, trying to connect...");
				Init ();
			}

			// Wait for 200ms and repeat
			Thread.Sleep(100);       
		}

		// Exit function
		public void Exit()
		{
			// We will never exit the application
		}

		/// Attach command call backs. 
		private void AttachCommandCallBacks()
		{
			_cmdMessenger.Attach(OnUnknownCommand);
			_cmdMessenger.Attach((int)Command.Status, OnStatus);
			_cmdMessenger.Attach((int)Command.Log, OnLog);
		}

		/// Executes when an unknown command has been received.
		void OnUnknownCommand(ReceivedCommand arguments)
		{
			Logger.Log ("Command received from Arduino without attached callback");
		}

		// Callback function that prints the Arduino status to the console
		void OnStatus(ReceivedCommand arguments)
		{
			Logger.Log ("Arduino: Status: " + arguments.ReadStringArg());
		}

		// Callback function that prints the Arduino status to the console
		void OnLog(ReceivedCommand arguments)
		{
			//LogWriter.WriteLine(arguments.ReadStringArg());
			File.AppendAllText(homePath+"/log_arduino.txt", arguments.ReadStringArg() + "\n");
			//Logger.Log ("Arduino: Log: " + arguments.ReadStringArg());
		}

		public static bool IsLinux
		{
			get
			{
				int p = (int) Environment.OSVersion.Platform;
				return (p == 4) || (p == 6) || (p == 128);
			}
		}
	}

	class CommandListener 
	{
		private Server _tcpListener;
		private CmdMessenger _cmdMessenger;

		public CommandListener(Server tcpListener, CmdMessenger cmdMessenger) 
		{
			_tcpListener = tcpListener;
			_cmdMessenger = cmdMessenger;

			// Add "ListChanged" to the Changed event on "List".
			_tcpListener.Received += new CommandReceivedEventHandler(CommandReceived);
		}

		// This will be called whenever the list changes.
		private void CommandReceived(object sender, TCPServer.Server.OnReceivedEventArgs e) 
		{
			Logger.Log ("Command Received: " + Regex.Replace (e.Command, "[\n\r\t]", "~"));
			try {
				JObject jObject = JObject.Parse (e.Command);
				string command = (string) jObject ["command"];
				var args = jObject ["args"];
				switch(command) {
				case "led":
					_cmdMessenger.SendCommand(new SendCommand((int)Command.SetLed, (bool)args[0]));
					Logger.Log ("Sending Command to Arduino: SetLed:" + (bool)args[0]);
					break;
				}
			} catch {
				Logger.Log ("Error Parsing Command");
			}


			//var arguments = JsonConvert.DeserializeObject(JSON);
			//switch (e.Command) {
			//case "led":
			//	break;
			//case "led_off":
			//	break;
			//}


			// Create command
			//var command = new SendCommand((int)Command.SetLed, _ledState);               

			// Send command
			//_cmdMessenger.SendCommand(command);
		}

		public void Detach() 
		{
			// Detach the event and delete the list
			_tcpListener.Received -= new CommandReceivedEventHandler(CommandReceived);
			_tcpListener = null;
		}
	}

}