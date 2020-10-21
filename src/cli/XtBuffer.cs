using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public sealed class XtBuffer
    {
        public double min;
        public double max;
        public double current;
    }
}