using System;
using System.Security;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    delegate void OnXRun(int index, IntPtr user);
    [SuppressUnmanagedCodeSecurity]
    delegate void OnRunning(IntPtr stream, int running, IntPtr user);
    [SuppressUnmanagedCodeSecurity]
    delegate int OnBuffer(IntPtr stream, in XtBuffer buffer, IntPtr user);
    [SuppressUnmanagedCodeSecurity]
    public delegate void XtOnError(in XtLocation location, string message);

    public delegate void XtOnXRun(int index, object user);
    public delegate void XtOnRunning(XtStream stream, bool running, object user);
    public delegate int XtOnBuffer(XtStream stream, in XtBuffer buffer, object user);
}