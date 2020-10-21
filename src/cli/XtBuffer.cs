using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public struct XtBuffer
    {
        public double min;
        public double max;
        public double current;
    }
}