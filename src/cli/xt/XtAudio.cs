using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public sealed class XtAudio : IDisposable
    {
        [DllImport("xt-core")] static extern void XtAudioTerminate();
        [DllImport("xt-core")] static extern XtVersion XtAudioGetVersion();
        [DllImport("xt-core")] static extern IntPtr XtAudioGetService(XtSystem system);
        [DllImport("xt-core")] static extern XtSystem XtAudioSetupToSystem(XtSetup setup);
        [DllImport("xt-core")] static extern XtErrorInfo XtAudioGetErrorInfo(ulong error);
        [DllImport("xt-core")] static extern XtAttributes XtAudioGetSampleAttributes(XtSample sample);
        [DllImport("xt-core")] static extern void XtAudioGetSystems([Out] XtSystem[] buffer, ref int size);
        [DllImport("xt-core")] static extern void XtAudioInit(byte[] id, IntPtr window, XtOnError onError);

        private XtAudio() { }
        static XtOnError _onError;

        public void Dispose() => XtAudioTerminate();
        public static XtVersion GetVersion() => XtAudioGetVersion();
        public static XtErrorInfo GetErrorInfo(ulong error) => XtAudioGetErrorInfo(error);
        public static XtSystem SetupToSystem(XtSetup setup) => XtAudioSetupToSystem(setup);
        public static XtAttributes GetSampleAttributes(XtSample sample) => XtAudioGetSampleAttributes(sample);

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

        public static IDisposable Init(string id, IntPtr window, XtOnError onError)
        {
            _onError = onError;
            XtAudioInit(Encoding.UTF8.GetBytes(id), window, onError);
            return new XtAudio();
        }
    }
}