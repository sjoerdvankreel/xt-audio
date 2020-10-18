namespace Xt
{
    public sealed class XtFormat
    {
        public XtMix mix = new XtMix();
        public XtChannels channels = new XtChannels();

        public XtFormat()
        {
        }

        public XtFormat(XtMix mix, XtChannels channels)
        {
            this.mix = mix;
            this.channels = channels;
        }

        public override string ToString() => XtPrint.FormatToString(this);
    }
}