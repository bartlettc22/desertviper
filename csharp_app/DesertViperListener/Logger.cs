using System;

namespace DesertViperListener
{
	public class Logger
	{
		public Logger()
		{
		}

		public static void Log(string message) 
		{
			Console.Write(string.Format("[{0:yyyy-MM-dd HH:mm:ss.fff}] ",DateTime.Now));
			Console.WriteLine(message);
		}
	}
}

