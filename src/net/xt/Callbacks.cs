using System;
using System.Security;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    delegate void OnXRun(IntPtr stream, int index, IntPtr user);
    [SuppressUnmanagedCodeSecurity]
    delegate int OnBuffer(IntPtr stream, in XtBuffer buffer, IntPtr user);
    [SuppressUnmanagedCodeSecurity]
    public delegate void XtOnError(in XtLocation location, string message);
    [SuppressUnmanagedCodeSecurity]
    delegate void OnRunning(IntPtr stream, int running, ulong error, IntPtr user);

    public delegate void XtOnXRun(XtStream stream, int index, object user);
    public delegate int XtOnBuffer(XtStream stream, in XtBuffer buffer, object user);
    public delegate void XtOnRunning(XtStream stream, bool running, ulong error, object user);
}