using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace DesertViperListener
{
	class MainClass
	{
		static void Main()
		{
			// mimics Arduino calling structure
			var program = new Program {RunLoop = true};
			program.Setup();
			while (program.RunLoop) program.Loop();
			program.Exit();
		}

	}
}
