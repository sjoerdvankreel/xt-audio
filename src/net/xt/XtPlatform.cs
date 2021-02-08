using System;
using System.Runtime.InteropServices;
using System.Security;
using static Xt.Utility;

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
        internal XtPlatform(IntPtr p) => _p = p;

        public void Dispose() { HandleAssert(() => XtPlatformDestroy(_p)); _p = IntPtr.Zero; }
        public XtSystem SetupToSystem(XtSetup setup) => HandleAssert(XtPlatformSetupToSystem(_p, setup));

        public XtSystem[] GetSystems()
        {
            int size = 0;
            HandleAssert(() => XtPlatformGetSystems(_p, null, ref size));
            var result = new XtSystem[size];
            HandleAssert(() => XtPlatformGetSystems(_p, result, ref size));
            return result;
        }

        public XtService GetService(XtSystem system)
        {
            IntPtr s = HandleAssert(XtPlatformGetService(_p, system));
            return s == IntPtr.Zero ? null : new XtService(s);
        }
    }
}