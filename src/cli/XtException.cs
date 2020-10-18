using System;

namespace Xt
{
    public sealed class XtException : Exception
    {
        private readonly ulong error;
        internal XtException(ulong error) => this.error = error;

        public ulong GetError() => error;
        public override string ToString() => XtPrint.ErrorToString(GetError());

        public static int GetFault(ulong error) => XtNative.XtErrorGetFault(error);
        public static XtCause GetCause(ulong error) => XtNative.XtErrorGetCause(error);
        public static XtSystem GetSystem(ulong error) => XtNative.XtErrorGetSystem(error);
        public static string GetText(ulong error) => XtNative.StringFromUtf8(XtNative.XtErrorGetText(error));
    }
}