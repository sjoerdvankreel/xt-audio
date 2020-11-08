using System;
using System.Runtime.InteropServices;
using static Xt.XtNative;

namespace Xt
{
	public sealed class XtStream : IDisposable
	{
		readonly IntPtr _s;
		internal IntPtr Handle() => _s;
		readonly XtXRunCallback _xRunCallback;
		readonly XtStreamCallback _streamCallback;
		internal XtStream(IntPtr s, XtStreamCallback streamCallback, XtXRunCallback xRunCallback)
		=> (_s, _streamCallback, _xRunCallback) = (s, streamCallback, xRunCallback);

		public void Dispose() => XtStreamDestroy(_s);
		public void Stop() => HandleError(XtStreamStop(_s));
		public void Start() => HandleError(XtStreamStart(_s));
		public unsafe XtFormat GetFormat() => *XtStreamGetFormat(_s);
		public int GetFrames() => HandleError(XtStreamGetFrames(_s, out var r), r);
		public XtLatency GetLatency() => HandleError(XtStreamGetLatency(_s, out var r), r);

		[DllImport("xt-core")] static extern ulong XtStreamStop(IntPtr s);
		[DllImport("xt-core")] static extern ulong XtStreamStart(IntPtr s);
		[DllImport("xt-core")] static extern void XtStreamDestroy(IntPtr s);
		[DllImport("xt-core")] static extern unsafe XtFormat* XtStreamGetFormat(IntPtr s);
		[DllImport("xt-core")] static extern ulong XtStreamGetFrames(IntPtr s, out int frames);
		[DllImport("xt-core")] static extern ulong XtStreamGetLatency(IntPtr s, out XtLatency latency);
	}
}