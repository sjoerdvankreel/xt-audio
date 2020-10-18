using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public sealed class XtAttributes
    {
        public int size;
        public bool isFloat;
        public bool isSigned;

        public override string ToString() => XtPrint.AttributesToString(this);
    }
}