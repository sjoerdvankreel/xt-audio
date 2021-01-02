using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    static class Utility
    {
        [DllImport("xt-core")] internal static extern IntPtr XtPrintErrorInfo(ref XtErrorInfo info);
        [DllImport("xt-core")] internal static extern IntPtr XtPrintLocation(ref XtLocation location);
        internal static bool HandleError(ulong error) => error == 0 ? true : throw new XtException(error);
        internal static T HandleError<T>(ulong error, T result) => error == 0 ? result : throw new XtException(error);

        internal static unsafe string PtrToStringUTF8(IntPtr ptr)
        {
            var range = Enumerable.Range(0, int.MaxValue);
            var length = range.SkipWhile(i => ((byte*)ptr)[i] != 0).First();
            return Encoding.UTF8.GetString((byte*)ptr, length);
        }
    }
}