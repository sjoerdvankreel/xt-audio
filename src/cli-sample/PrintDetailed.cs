using System;

namespace Xt
{
	public static class PrintDetailed
	{
		static void OnFatal() => Console.WriteLine("Fatal error.");

		static void OnTrace(XtLevel level, String message)
		{
			if (level != XtLevel.Info)
				Console.WriteLine("-- " + level + ": " + message);
		}

		public static void Main()
		{
			using var audio = new XtAudio("Sample", IntPtr.Zero, OnTrace, OnFatal);
			try
			{
				var version = XtAudio.GetVersion();
				Console.WriteLine("Version: " + version.major + "." + version.minor);
				XtService pro = XtAudio.GetServiceBySetup(XtSetup.ProAudio);
				if (pro != null) Console.WriteLine("Pro Audio: " + pro);
				XtService system = XtAudio.GetServiceBySetup(XtSetup.SystemAudio);
				if (system != null) Console.WriteLine("System Audio: " + system);
				XtService consumer = XtAudio.GetServiceBySetup(XtSetup.ConsumerAudio);
				if (consumer != null) Console.WriteLine("Consumer Audio: " + consumer);

				for (int s = 0; s < XtAudio.GetServiceCount(); s++)
				{
					XtService service = XtAudio.GetServiceByIndex(s);
					Console.WriteLine("Service " + service + ":");
					Console.WriteLine("  System: " + service.GetSystem());
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