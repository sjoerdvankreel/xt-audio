using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public sealed class XtChannels
    {
        public int inputs;
        public ulong inMask;
        public int outputs;
        public ulong outMask;

        public XtChannels()
        {
        }

        public XtChannels(int inputs, ulong inMask, int outputs, ulong outMask)
        {
            this.inputs = inputs;
            this.inMask = inMask;
            this.outputs = outputs;
            this.outMask = outMask;
        }
    }
}