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
        internal static extern IntPtr XtPrintErrorInfoToString(ref XtErrorInfo info);
    }
}