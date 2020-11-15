using System;
using static Xt.XtNative;

namespace Xt
{
	public sealed class XtAudio : IDisposable
	{
		static XtOnError _onError;
		public void Dispose() => XtAudioTerminate();
		public static XtVersion GetVersion() => XtAudioGetVersion();
		public static XtErrorInfo GetErrorInfo(ulong error) => XtAudioGetErrorInfo(error);
		public static XtSystem SetupToSystem(XtSetup setup) => XtAudioSetupToSystem(setup);
		public static XtAttributes GetSampleAttributes(XtSample sample) => XtAudioGetSampleAttributes(sample);

		public XtAudio(string id, IntPtr window, XtOnError onError)
		{
			_onError = onError;
			XtAudioInit(id, window, onError);
		}

		public static XtSystem[] GetSystems()
		{
			int size = 0;
			XtAudioGetSystems(null, ref size);
			var result = new XtSystem[size];
			XtAudioGetSystems(result, ref size);
			return result;
		}

		public static XtService GetService(XtSystem system)
		{
			IntPtr service = XtAudioGetService(system);
			return service == IntPtr.Zero ? null : new XtService(service);
		}
	}
}