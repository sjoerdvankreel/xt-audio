using System;
using System.Runtime.InteropServices;
using System.Security;

namespace Xt
{
	[SuppressUnmanagedCodeSecurity]
	static class XtNative
	{
		internal static bool HandleError(ulong error) => error == 0 ? true : throw new XtException(error);
		internal static T HandleError<T>(ulong error, T result) => error == 0 ? result : throw new XtException(error);

		private const int RTLD_NOW = 2;
		[DllImport("kernel32")]
		private static extern IntPtr LoadLibrary(string library);
		[DllImport("libdl")]
		private static extern IntPtr dlopen(string filename, int flags);

		static XtNative()
		{
			if (Environment.OSVersion.Platform == PlatformID.Win32NT)
				if (LoadLibrary(Environment.Is64BitProcess ? "win32-x64/xt-core.dll" : "win32-x86/xt-core.dll") == IntPtr.Zero)
					throw new DllNotFoundException();
			if (Environment.OSVersion.Platform == PlatformID.Unix)
				if (dlopen(Environment.Is64BitProcess ? "linux-x64/libxt-core.so" : "linux-x86/libxt-core.so", RTLD_NOW) == IntPtr.Zero)
					throw new DllNotFoundException();
		}

		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern XtErrorInfo XtAudioGetErrorInfo(ulong error);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern IntPtr XtPrintErrorInfoToString(ref XtErrorInfo info);

		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern XtCapabilities XtServiceGetCapabilities(IntPtr s);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtServiceGetDeviceCount(IntPtr s, out int count);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtServiceOpenDevice(IntPtr s, int index, out IntPtr device);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtServiceOpenDefaultDevice(IntPtr s, bool output, out IntPtr device);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtServiceAggregateStream(IntPtr s,
			IntPtr[] devices, XtChannels[] channels,
			double[] bufferSizes, int count,
			in XtMix mix, bool interleaved, IntPtr master, XtStreamCallback streamCallback,
			XtXRunCallback xRunCallback, IntPtr user, out IntPtr stream);

		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern void XtAudioTerminate();
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern XtVersion XtAudioGetVersion();
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern void XtAudioGetSystems([Out] XtSystem[] buffer, ref int size);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern XtSystem XtAudioSetupToSystem(XtSetup setup);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern IntPtr XtAudioGetService(XtSystem system);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern XtAttributes XtAudioGetSampleAttributes(XtSample sample);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern void XtAudioInit([MarshalAs(UnmanagedType.LPUTF8Str)] string id, IntPtr window, XtErrorCallback error);
	}
}