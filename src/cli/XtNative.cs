using System;
using System.Runtime.InteropServices;
using System.Security;

namespace Xt
{
	[SuppressUnmanagedCodeSecurity]
	static class XtNative
	{
		[SuppressUnmanagedCodeSecurity]
		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		internal delegate void StreamCallbackWin32(IntPtr stream, IntPtr input, IntPtr output,
			int frames, double time, ulong position, bool timeValid, ulong error, IntPtr user);

		[SuppressUnmanagedCodeSecurity]
		internal delegate void StreamCallbackLinux(IntPtr stream, IntPtr input, IntPtr output,
			int frames, double time, ulong position, bool timeValid, ulong error, IntPtr user);

		[SuppressUnmanagedCodeSecurity]
		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		internal delegate void XRunCallbackWin32(int index, IntPtr user);

		[SuppressUnmanagedCodeSecurity]
		internal delegate void XRunCallbackLinux(int index, IntPtr user);

		internal static bool HandleError(ulong error) => error == 0 ? true : throw new XtException(error);

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
		internal static extern void XtStreamDestroy(IntPtr s);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtStreamStop(IntPtr s);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtStreamStart(IntPtr s);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtStreamGetFrames(IntPtr s, out int frames);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtStreamGetLatency(IntPtr s, out XtLatency latency);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern IntPtr XtStreamGetFormat(IntPtr s);

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
			[MarshalAs(UnmanagedType.LPArray)] IntPtr[] devices, IntPtr channels,
			[MarshalAs(UnmanagedType.LPArray)] double[] bufferSizes, int count,
			XtMix mix, bool interleaved, IntPtr master, IntPtr streamCallback,
			IntPtr xRunCallback, IntPtr user, out IntPtr stream);

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

		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern void XtDeviceDestroy(IntPtr d);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceShowControlPanel(IntPtr d);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceGetMix(IntPtr d, out bool valid, out XtMix mix);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceGetName(IntPtr d, [Out] byte[] buffer, ref int size);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceGetChannelCount(IntPtr d, bool output, out int count);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceSupportsAccess(IntPtr d, bool interleaved, out bool supports);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceGetBufferSize(IntPtr d, in XtFormat format, out XtBufferSize size);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceSupportsFormat(IntPtr d, in XtFormat format, out bool supports);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceGetChannelName(IntPtr d, bool output, int index, [Out] byte[] buffer, ref int size);
		[DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
		internal static extern ulong XtDeviceOpenStream(IntPtr d, in XtFormat format, bool interleaved,
			double bufferSize, IntPtr streamCallback, IntPtr xRunCallback, IntPtr user, out IntPtr stream);
	}
}