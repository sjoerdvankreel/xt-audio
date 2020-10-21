using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public struct XtFormat
    {
        public XtMix mix;
        public XtChannels channels;

        public XtFormat(XtMix mix, XtChannels channels)
        {
            this.mix = mix;
            this.channels = channels;
        }
    }
}