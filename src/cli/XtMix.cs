using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public struct XtMix
    {
        public int rate;
        public XtSample sample;

        public XtMix(int rate, XtSample sample)
        {
            this.rate = rate;
            this.sample = sample;
        }
    }
}