using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using static Xt.Utility;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public static class XtAudio
    {
        const int RTLD_NOW = 2;
        [DllImport("kernel32.dll")]
        static extern IntPtr LoadLibrary(string file);
        [DllImport("libc")]
        static extern IntPtr dlopen(string filename, int flags);

        [DllImport("xt-audio")]
        static extern IntPtr XtAudioGetLastAssert();
        [DllImport("xt-audio")]
        static extern XtVersion XtAudioGetVersion();
        [DllImport("xt-audio")]
        static extern void XtAudioSetOnError(XtOnError onError);
        [DllImport("xt-audio")]
        static extern XtErrorInfo XtAudioGetErrorInfo(ulong error);
        [DllImport("xt-audio")]
        static extern IntPtr XtAudioInit(byte[] id, IntPtr window);
        [DllImport("xt-audio")]
        static extern void XtAudioSetAssertTerminates(int terminates);
        [DllImport("xt-audio")]
        static extern XtAttributes XtAudioGetSampleAttributes(XtSample sample);

        static XtOnError _onError;

        static XtAudio()
        {
            string location = Path.GetDirectoryName(typeof(XtAudio).Assembly.Location);
            string path = Path.Combine(location, "x64");
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
                if (LoadLibrary(Path.Combine(path, "xt-audio.dll")) == IntPtr.Zero)
                    throw new DllNotFoundException();
            if (Environment.OSVersion.Platform == PlatformID.Unix)
                if (dlopen(Path.Combine(path, "libxt-audio.so"), RTLD_NOW) == IntPtr.Zero)
                    throw new DllNotFoundException();
            XtAudioSetAssertTerminates(0);
        }

        public static XtVersion GetVersion()
        => HandleAssert(XtAudioGetVersion());
        public static XtErrorInfo GetErrorInfo(ulong error)
        => HandleAssert(XtAudioGetErrorInfo(error));
        public static XtAttributes GetSampleAttributes(XtSample sample)
        => HandleAssert(XtAudioGetSampleAttributes(sample));

        public static void SetOnError(XtOnError onError)
        {
            _onError = onError;
            HandleAssert(() => XtAudioSetOnError(onError));
        }

        internal static string GetLastAssert()
        {
            IntPtr assert = XtAudioGetLastAssert();
            return assert == IntPtr.Zero ? null : PtrToStringUTF8(assert);
        }

        public static XtPlatform Init(string id, IntPtr window)
        {
            byte[] idBytes = Encoding.UTF8.GetBytes(id + char.MinValue);
            return new XtPlatform(HandleAssert(XtAudioInit(idBytes, window)));
        }
    }
}