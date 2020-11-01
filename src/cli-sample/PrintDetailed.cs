using System;

namespace Xt
{
	public static class PrintDetailed
	{
		static void OnError(string location, string message) => Console.WriteLine($"{location}: {message}");

		public static void Main()
		{
			using var audio = new XtAudio("Sample", IntPtr.Zero, OnError);
			try
			{
				var version = XtAudio.GetVersion();
				Console.WriteLine("Version: " + version.major + "." + version.minor);
				var pro = XtAudio.SetupToSystem(XtSetup.ProAudio);
				Console.WriteLine("Pro Audio: " + pro + " (" + (XtAudio.GetService(pro) != null) + ")");
				var system = XtAudio.SetupToSystem(XtSetup.SystemAudio);
				Console.WriteLine("System Audio: " + system + " (" + (XtAudio.GetService(system) != null) + ")");
				var consumer = XtAudio.SetupToSystem(XtSetup.ConsumerAudio);
				Console.WriteLine("Consumer Audio: " + consumer + " (" + (XtAudio.GetService(consumer) != null) + ")");

				foreach(var s in XtAudio.GetSystems())
				{
					XtService service = XtAudio.GetService(s);
					Console.WriteLine("System: " + s);
					Console.WriteLine("  Device count: " + service.GetDeviceCount());
					Console.WriteLine("  Capabilities: " + service.GetCapabilities());
					using var defaultInput = service.OpenDefaultDevice(false);
					if (defaultInput != null) Console.WriteLine("  Default input: " + defaultInput);
					using var defaultOutput = service.OpenDefaultDevice(true);
					if (defaultOutput != null) Console.WriteLine("  Default output: " + defaultOutput);

					for (int d = 0; d < service.GetDeviceCount(); d++)
						using (XtDevice device = service.OpenDevice(d))
						{
							var mix = device.GetMix();
							Console.WriteLine("  Device " + device + ":");
							Console.WriteLine("    Input channels: " + device.GetChannelCount(false));
							Console.WriteLine("    Output channels: " + device.GetChannelCount(true));
							Console.WriteLine("    Interleaved access: " + device.SupportsAccess(true));
							Console.WriteLine("    Non-interleaved access: " + device.SupportsAccess(false));
							if (mix != null) Console.WriteLine("    Current mix: " + mix.Value.rate + " " + mix.Value.sample);
						}
				}
			}
			catch (XtException e)
			{
				var error = XtAudio.GetErrorInfo(e.GetError());
				Console.WriteLine("Error: system {0}, fault {1}, cause {2}, text {3}.\n",
					error.system, error.fault, error.cause, error.text);
			}
		}
	}
}