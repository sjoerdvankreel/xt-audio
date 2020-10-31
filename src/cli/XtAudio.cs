using System;
using System.Runtime.InteropServices;
using static Xt.XtNative;

namespace Xt
{
	public sealed class XtAudio : IDisposable
	{
		internal static XtTraceCallback trace;
		private static TraceCallbackWin32 win32Trace;
		private static TraceCallbackLinux linuxTrace;
		private static XtFatalCallback fatal;
		private static FatalCallbackWin32 win32Fatal;
		private static FatalCallbackLinux linuxFatal;

		public XtAudio(string id, IntPtr window, XtTraceCallback trace, XtFatalCallback fatal)
		{
			XtAudio.trace = trace;
			XtAudio.win32Trace = trace == null ? null : new XtNative.TraceCallbackWin32(trace);
			XtAudio.linuxTrace = trace == null ? null : new XtNative.TraceCallbackLinux(trace);
			XtAudio.fatal = fatal;
			XtAudio.win32Fatal = fatal == null ? null : new XtNative.FatalCallbackWin32(fatal);
			XtAudio.linuxFatal = fatal == null ? null : new XtNative.FatalCallbackLinux(fatal);
			Delegate traceDelegate = Environment.OSVersion.Platform == PlatformID.Win32NT ? (Delegate)win32Trace : linuxTrace;
			Delegate fatalDelegate = Environment.OSVersion.Platform == PlatformID.Win32NT ? (Delegate)win32Fatal : linuxFatal;
			IntPtr tracePtr = trace == null ? IntPtr.Zero : Marshal.GetFunctionPointerForDelegate(traceDelegate);
			IntPtr fatalPtr = fatal == null ? IntPtr.Zero : Marshal.GetFunctionPointerForDelegate(fatalDelegate);
			XtNative.XtAudioInit(id, window, tracePtr, fatalPtr);
		}

		public void Dispose() => XtAudioTerminate();
		public static XtVersion GetVersion() => XtAudioGetVersion();
		public static XtErrorInfo GetErrorInfo(ulong error) => XtAudioGetErrorInfo(error);
		public static XtSystem SetupToSystem(XtSetup setup) => XtAudioSetupToSystem(setup);
		public static XtAttributes GetSampleAttributes(XtSample sample) => XtAudioGetSampleAttributes(sample);

		public static XtService GetService(XtSystem system)
		{
			IntPtr service = XtAudioGetService(system);
			return service == IntPtr.Zero ? null : new XtService(service);
		}

		public static XtSystem[] GetSystems()
		{
			int size = 0;
			XtAudioGetSystems(null, ref size);
			var result = new XtSystem[size];
			XtAudioGetSystems(result, ref size);
			return result;
		}
	}
}