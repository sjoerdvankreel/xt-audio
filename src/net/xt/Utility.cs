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
        [DllImport("xt-audio")]
        internal static extern IntPtr XtPrintErrorInfo(ref XtErrorInfo info);

        internal static T HandleAssert<T>(T result)
        {
            HandleAssert();
            return result;
        }

        internal static bool HandleError(ulong error)
        {
            HandleAssert();
            if (error != 0) throw new XtException(error);
            return true;
        }

        internal static void HandleAssert(Action action)
        {
            action();
            HandleAssert();
        }

        internal static T HandleError<T>(ulong error, T result)
        {
            HandleAssert();
            if (error != 0) throw new XtException(error);
            return result;
        }

        static void HandleAssert()
        {
            var assert = XtAudio.GetLastAssert();
            if (assert != null) throw new InvalidOperationException(assert);
        }

        internal static unsafe string PtrToStringUTF8(IntPtr ptr)
        {
            var range = Enumerable.Range(0, int.MaxValue);
            var length = range.SkipWhile(i => ((byte*)ptr)[i] != 0).First();
            return Encoding.UTF8.GetString((byte*)ptr, length);
        }
    }
}