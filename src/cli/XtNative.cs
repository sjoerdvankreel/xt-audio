using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    static class XtNative
    {
        internal class Utf8Buffer : IDisposable
        {
            internal readonly IntPtr ptr = IntPtr.Zero;

            internal Utf8Buffer(string s)
            {
                if (s == null)
                    return;
                byte[] bytes = Encoding.UTF8.GetBytes(s);
                ptr = Marshal.AllocHGlobal(bytes.Length);
                Marshal.Copy(bytes, 0, ptr, bytes.Length);
            }

            public void Dispose()
            {
                if (ptr != IntPtr.Zero)
                    Marshal.FreeHGlobal(ptr);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct Format
        {
            public int rate;
            public XtSample sample;
            public int inputs;
            public ulong inMask;
            public int outputs;
            public ulong outMask;

            public XtFormat FromNative()
            {
                XtFormat result = new XtFormat();
                result.mix.rate = rate;
                result.mix.sample = sample;
                result.channels.inputs = inputs;
                result.channels.inMask = inMask;
                result.channels.outputs = outputs;
                result.channels.outMask = outMask;
                return result;
            }

            public static Format ToNative(XtFormat format)
            {
                Format result = new Format();
                result.rate = format.mix.rate;
                result.sample = format.mix.sample;
                result.inputs = format.channels.inputs;
                result.inMask = format.channels.inMask;
                result.outputs = format.channels.outputs;
                result.outMask = format.channels.outMask;
                return result;
            }
        }

        [SuppressUnmanagedCodeSecurity]
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        internal delegate void FatalCallbackWin32();

        [SuppressUnmanagedCodeSecurity]
        internal delegate void FatalCallbackLinux();

        [SuppressUnmanagedCodeSecurity]
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        internal delegate void TraceCallbackWin32(XtLevel level, string message);

        [SuppressUnmanagedCodeSecurity]
        internal delegate void TraceCallbackLinux(XtLevel level, string message);

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

        internal static void HandleError(ulong error)
        {
            if (error != 0)
                throw new XtException(error);
        }

        internal static string FreeStringFromUtf8(IntPtr utf8)
        {
            string result = StringFromUtf8(utf8);
            XtAudioFree(utf8);
            return result;
        }

        internal static string StringFromUtf8(IntPtr utf8)
        {
            byte c;
            int index = 0;
            List<byte> bytes = new List<byte>();
            while ((c = Marshal.ReadByte(utf8, index++)) != 0)
                bytes.Add(c);
            byte[] array = bytes.ToArray();
            return Encoding.UTF8.GetString(array);
        }

        private const int RTLD_NOW = 2;
        [DllImport("kernel32")]
        private static extern IntPtr LoadLibrary(string library);
        [DllImport("libdl")]
        private static extern IntPtr dlopen(string filename, int flags);

        static XtNative()
        {
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
                if(LoadLibrary(Environment.Is64BitProcess ? "win32-x64/xt-core.dll" : "win32-x86/xt-core.dll") == IntPtr.Zero)
                    throw new DllNotFoundException();
            if(Environment.OSVersion.Platform == PlatformID.Unix)
                if(dlopen(Environment.Is64BitProcess ? "linux-x64/libxt-core.so" : "linux-x86/libxt-core.so", RTLD_NOW) == IntPtr.Zero)
                    throw new DllNotFoundException();
        }

        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern XtCause XtErrorGetCause(ulong error);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern int XtErrorGetFault(ulong error);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern XtSystem XtErrorGetSystem(ulong error);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtErrorGetText(ulong error);

        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintErrorToString(ulong error);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintMixToString(XtMix mix);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintLevelToString(XtLevel level);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintCauseToString(XtCause cause);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintSetupToString(XtSetup setup);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintSystemToString(XtSystem system);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintSampleToString(XtSample sample);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintFormatToString(ref Format format);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintBufferToString(XtBuffer buffer);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintLatencyToString(XtLatency latency);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintChannelsToString(XtChannels channels);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintAttributesToString(in XtAttributes attributes);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtPrintCapabilitiesToString(XtCapabilities capabilities);

        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern void XtStreamDestroy(IntPtr s);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtStreamStop(IntPtr s);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtStreamStart(IntPtr s);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern XtSystem XtStreamGetSystem(IntPtr s);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtStreamGetFrames(IntPtr s, out int frames);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtStreamGetLatency(IntPtr s, [In, Out] XtLatency latency);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtStreamGetFormat(IntPtr s);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern bool XtStreamIsInterleaved(IntPtr s);

        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern XtSystem XtServiceGetSystem(IntPtr s);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtServiceGetName(IntPtr s);
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
        internal static extern bool XtAudioIsWin32();
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern void XtAudioTerminate();
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern void XtAudioFree(IntPtr p);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern int XtAudioGetVersionMajor();
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern int XtAudioGetVersionMinor();
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern int XtAudioGetServiceCount();
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtAudioGetServiceByIndex(int index);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtAudioGetServiceBySetup(XtSetup setup);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr XtAudioGetServiceBySystem(XtSystem system);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern XtAttributes XtAudioGetSampleAttributes(XtSample sample);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern void XtAudioInit(IntPtr id, IntPtr window, IntPtr trace, IntPtr fatal);

        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern void XtDeviceDestroy(IntPtr d);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceShowControlPanel(IntPtr d);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern XtSystem XtDeviceGetSystem(IntPtr d);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceGetMix(IntPtr d, out IntPtr mix);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceGetName(IntPtr d, out IntPtr name);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceGetChannelCount(IntPtr d, bool output, out int count);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceSupportsAccess(IntPtr d, bool interleaved, out bool supports);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceGetBuffer(IntPtr d, ref Format format, [In, Out] XtBuffer buffer);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceSupportsFormat(IntPtr d, ref Format format, out bool supports);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceGetChannelName(IntPtr d, bool output, int index, out IntPtr name);
        [DllImport("xt-core", CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong XtDeviceOpenStream(IntPtr d, ref Format format, bool interleaved,
            double bufferSize, IntPtr streamCallback, IntPtr xRunCallback, IntPtr user, out IntPtr stream);
    }
}