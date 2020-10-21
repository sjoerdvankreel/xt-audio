using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public sealed class XtMix
    {
        public int rate;
        public XtSample sample;

        public XtMix()
        {
        }

        public XtMix(int rate, XtSample sample)
        {
            this.rate = rate;
            this.sample = sample;
        }
    }
}