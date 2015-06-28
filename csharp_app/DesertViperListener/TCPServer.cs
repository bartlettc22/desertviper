using System;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using System.Net;
using CommandMessenger;
using CommandMessenger.Transport.Serial;
using DesertViperListener;
using System.Text.RegularExpressions;

namespace TCPServer
{

	// A delegate type for hooking up command notifications.
	public delegate void CommandReceivedEventHandler(object sender, TCPServer.Server.OnReceivedEventArgs e);

    public class Server
    {
        private TcpListener tcpListener;
        private Thread listenThread;

		// An event that clients can use to be notified whenever TCP Server 
		// receives a message
		public event CommandReceivedEventHandler Received;

        public Server()
        {
            this.tcpListener = new TcpListener(IPAddress.Any, 3000);
			Logger.Log("TCP Listener Started");
            this.listenThread = new Thread(new ThreadStart(ListenForClients));
            this.listenThread.Start();
        }

        private void ListenForClients()
        {
            this.tcpListener.Start();
			Logger.Log("Listening for new clients...");
            while (true)
            {
                //blocks until a client has connected to the server
                TcpClient client = this.tcpListener.AcceptTcpClient();
				Logger.Log("New client connected " + client.Client.RemoteEndPoint.ToString ());

                //create a thread to handle communication 
                //with connected client
                Thread clientThread = new Thread(new ParameterizedThreadStart(HandleClientComm));
                clientThread.Start(client);
            }
        }

        private void HandleClientComm(object client)
        {
			
            TcpClient tcpClient = (TcpClient)client;
            NetworkStream clientStream = tcpClient.GetStream();
			byte[] buffer = new byte[tcpClient.ReceiveBufferSize];
            int bytesRead;

            while (true)
            {
                bytesRead = 0;

                try
                {
                    //blocks until a client sends a message
					bytesRead = clientStream.Read(buffer, 0, tcpClient.ReceiveBufferSize);
                }
                catch
                {
                    //a socket error has occured
                    break;
                }

                if (bytesRead == 0)
                {
                    //the client has disconnected from the server
                    break;
                }

                //message has successfully been received
                ASCIIEncoding encoder = new ASCIIEncoding();
				System.Diagnostics.Debug.WriteLine(encoder.GetString(buffer, 0, bytesRead));

				// Trigger Received Event
				OnReceivedEventArgs args = new OnReceivedEventArgs();
				args.Command = encoder.GetString (buffer, 0, bytesRead);
				OnReceived(args); //encoder.GetString(buffer, 0, bytesRead)
            }

            tcpClient.Close();
        }

		// Invoke the Received event; called whenever TCP message is received
		protected virtual void OnReceived(OnReceivedEventArgs e) 
		{
			if (Received != null)
				Received(this, e);
		}

		public class OnReceivedEventArgs : EventArgs
		{
			public string Command { get; set; }
		}
    }
}
