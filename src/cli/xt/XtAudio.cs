using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public static class XtAudio
    {
        [DllImport("xt-core")] static extern XtVersion XtAudioGetVersion();
        [DllImport("xt-core")] static extern XtSystem XtAudioSetupToSystem(XtSetup setup);
        [DllImport("xt-core")] static extern XtErrorInfo XtAudioGetErrorInfo(ulong error);
        [DllImport("xt-core")] static extern XtAttributes XtAudioGetSampleAttributes(XtSample sample);
        [DllImport("xt-core")] static extern IntPtr XtAudioInit(byte[] id, IntPtr window, XtOnError onError);

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