using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public struct XtLatency
    {
        public double input;
        public double output;
    }
}