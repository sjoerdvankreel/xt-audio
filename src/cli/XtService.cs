using System;
using System.Linq;
using static Xt.XtNative;

namespace Xt
{
	public sealed class XtService
	{
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
			bool interleaved, XtDevice master, XtStreamCallback streamCallback, XtXRunCallback xRunCallback, IntPtr user)
		=> HandleError(XtServiceAggregateStream(_s, devices.Select(d => d.Handle).ToArray(), channels, bufferSizes, count, in mix, 
			interleaved, master.Handle, streamCallback, xRunCallback, user, out var r), new XtStream(r, streamCallback, xRunCallback));
	}
}