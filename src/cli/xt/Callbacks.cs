using System;
using System.Security;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    delegate void OnXRun(int index, IntPtr user);
    [SuppressUnmanagedCodeSecurity]
    delegate void OnBuffer(IntPtr stream, in XtBuffer buffer, IntPtr user);
    [SuppressUnmanagedCodeSecurity]
    public delegate void XtOnError(in XtLocation location, string message);

    public delegate void XtOnXRun(int index, object user);
    public delegate void XtOnBuffer(XtStream stream, in XtBuffer buffer, object user);
}