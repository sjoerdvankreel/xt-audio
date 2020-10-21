using System;
using System.Runtime.InteropServices;

namespace Xt
{
    public sealed class XtAudio : IDisposable
    {
        internal static XtTraceCallback trace;
        private static XtNative.TraceCallbackWin32 win32Trace;
        private static XtNative.TraceCallbackLinux linuxTrace;
        private static XtFatalCallback fatal;
        private static XtNative.FatalCallbackWin32 win32Fatal;
        private static XtNative.FatalCallbackLinux linuxFatal;

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
            using (XtNative.Utf8Buffer buffer = new XtNative.Utf8Buffer(id))
                XtNative.XtAudioInit(buffer.ptr, window, tracePtr, fatalPtr);
        }

        public void Dispose() => XtNative.XtAudioTerminate();
        public static bool IsWin32() => XtNative.XtAudioIsWin32();
        public static int GetServiceCount() => XtNative.XtAudioGetServiceCount();
        public static int GetVersionMajor() => XtNative.XtAudioGetVersionMajor();
        public static int GetVersionMinor() => XtNative.XtAudioGetVersionMinor();
        public static int GetErrorFault(ulong error) => XtNative.XtAudioGetErrorFault(error);
        public static XtCause GetErrorCause(ulong error) => XtNative.XtAudioGetErrorCause(error);
        public static XtSystem GetErrorSystem(ulong error) => XtNative.XtAudioGetErrorSystem(error);
        public static string GetErrorText(ulong error) => XtNative.StringFromUtf8(XtNative.XtAudioGetErrorText(error));

        public static XtService GetServiceByIndex(int index) => new XtService(XtNative.XtAudioGetServiceByIndex(index));

        public static XtService GetServiceBySetup(XtSetup setup)
        {
            IntPtr service = XtNative.XtAudioGetServiceBySetup(setup);
            return service == IntPtr.Zero ? null : new XtService(service);
        }

        public static XtService GetServiceBySystem(XtSystem system)
        {
            IntPtr service = XtNative.XtAudioGetServiceBySystem(system);
            return service == IntPtr.Zero ? null : new XtService(service);
        }

        public static XtAttributes GetSampleAttributes(XtSample sample) => XtNative.XtAudioGetSampleAttributes(sample);
    }
}