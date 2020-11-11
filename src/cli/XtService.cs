using System;
using System.Linq;
using System.Runtime.InteropServices;
using static Xt.XtNative;

namespace Xt
{
    public sealed class XtService
    {
        [DllImport("xt-core")] static extern XtCapabilities XtServiceGetCapabilities(IntPtr s);
        [DllImport("xt-core")] static extern ulong XtServiceGetDeviceCount(IntPtr s, out int count);
        [DllImport("xt-core")] static extern ulong XtServiceOpenDevice(IntPtr s, int index, out IntPtr device);
        [DllImport("xt-core")] static extern ulong XtServiceOpenDefaultDevice(IntPtr s, bool output, out IntPtr device);
        [DllImport("xt-core")] static extern ulong XtServiceAggregateStream(IntPtr s, IntPtr[] devices, XtChannels[] channels, double[] bufferSizes, int count, in XtMix mix, bool interleaved, IntPtr master, StreamCallback streamCallback, XRunCallback xRunCallback, IntPtr user, out IntPtr stream);

        readonly IntPtr _s;
        internal XtService(IntPtr s) => _s = s;

        public XtCapabilities GetCapabilities()
            => XtServiceGetCapabilities(_s);
        public int GetDeviceCount()
            => HandleError(XtServiceGetDeviceCount(_s, out var r), r);
        public XtDevice OpenDevice(int index)
            => HandleError(XtServiceOpenDevice(_s, index, out var r), new XtDevice(r));
        public XtDevice OpenDefaultDevice(bool output)
            => HandleError(XtServiceOpenDefaultDevice(_s, output, out var r), new XtDevice(r));

        public XtStream AggregateStream(XtDevice[] devices, XtChannels[] channels, double[] bufferSizes, int count, in XtMix mix,
            bool interleaved, XtDevice master, XtStreamCallback streamCallback, XtXRunCallback xRunCallback, object user)
        {
            var result = new XtStream(streamCallback, xRunCallback, user);
            result.Init(HandleError(XtServiceAggregateStream(_s, devices.Select(d => d.Handle()).ToArray(), channels, bufferSizes, count, in mix,
            interleaved, master.Handle(), result.NativeStreamCallback(), result.NativeXRunCallback(), IntPtr.Zero, out var r), r));
            return result;
        }
    }
}