using System;
using System.Runtime.InteropServices;
using System.Security;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public sealed class XtPlatform : IDisposable
    {
        [DllImport("xt-audio")] static extern void XtPlatformDestroy(IntPtr p);
        [DllImport("xt-audio")] static extern IntPtr XtPlatformGetService(IntPtr p, XtSystem system);
        [DllImport("xt-audio")] static extern XtSystem XtPlatformSetupToSystem(IntPtr p, XtSetup setup);
        [DllImport("xt-audio")] static extern void XtPlatformGetSystems(IntPtr p, [Out] XtSystem[] buffer, ref int size);

        IntPtr _p;
        readonly XtOnError _onError;
        internal XtPlatform(IntPtr p, XtOnError onError) => (_p, _onError) = (p, onError);

        public void Dispose() { XtPlatformDestroy(_p); _p = IntPtr.Zero; }
        public XtSystem SetupToSystem(XtSetup setup) => XtPlatformSetupToSystem(_p, setup);

        public XtSystem[] GetSystems()
        {
            int size = 0;
            XtPlatformGetSystems(_p, null, ref size);
            var result = new XtSystem[size];
            XtPlatformGetSystems(_p, result, ref size);
            return result;
        }

        public XtService GetService(XtSystem system)
        {
            IntPtr s = XtPlatformGetService(_p, system);
            return s == IntPtr.Zero ? null : new XtService(s);
        }
    }
}