using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public static class XtAudio
    {
        const int RTLD_NOW = 2;
        [DllImport("kernel32.dll")] static extern IntPtr LoadLibrary(string file);
        [DllImport("libdl.so")] static extern IntPtr dlopen(string filename, int flags);

        [DllImport("xt-core")] static extern XtVersion XtAudioGetVersion();
        [DllImport("xt-core")] static extern XtSystem XtAudioSetupToSystem(XtSetup setup);
        [DllImport("xt-core")] static extern XtErrorInfo XtAudioGetErrorInfo(ulong error);
        [DllImport("xt-core")] static extern XtAttributes XtAudioGetSampleAttributes(XtSample sample);
        [DllImport("xt-core")] static extern IntPtr XtAudioInit(byte[] id, IntPtr window, XtOnError onError);

        static XtAudio()
        {
            string prefix = IntPtr.Size == 4 ? "x86" : "x64";
            string location = Path.GetDirectoryName(typeof(XtAudio).Assembly.Location);
            string path = Path.Combine(location, prefix);
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
                if (LoadLibrary(Path.Combine(path, "xt-core.dll")) == IntPtr.Zero)
                    throw new DllNotFoundException();
            if (Environment.OSVersion.Platform == PlatformID.Unix)
                if (dlopen(Path.Combine(path, "libxt-core.so"), RTLD_NOW) == IntPtr.Zero)
                    throw new DllNotFoundException();
        }

        public static XtVersion GetVersion() => XtAudioGetVersion();
        public static XtErrorInfo GetErrorInfo(ulong error) => XtAudioGetErrorInfo(error);
        public static XtSystem SetupToSystem(XtSetup setup) => XtAudioSetupToSystem(setup);
        public static XtAttributes GetSampleAttributes(XtSample sample) => XtAudioGetSampleAttributes(sample);

        public static XtPlatform Init(string id, IntPtr window, XtOnError onError)
        {
            byte[] idBytes = Encoding.UTF8.GetBytes(id + char.MinValue);
            return new XtPlatform(XtAudioInit(idBytes, window, onError), onError);
        }
    }
}