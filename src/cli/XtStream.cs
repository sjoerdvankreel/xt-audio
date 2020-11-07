using System;
using static Xt.XtNative;

namespace Xt
{
	public class XtStream : IDisposable
	{
		IntPtr _s;
		readonly XtXRunCallback _xRunCallback;
		readonly XtStreamCallback _streamCallback;
		internal XtStream(IntPtr s, XtStreamCallback streamCallback, XtXRunCallback xRunCallback)
			=> (_s, _xRunCallback, _streamCallback) = (s, xRunCallback, streamCallback);

		public void Dispose() => XtStreamDestroy(_s);
		public void Stop() => HandleError(XtStreamStop(_s));
		public void Start() => HandleError(XtStreamStart(_s));
		public unsafe XtFormat GetFormat() => *XtStreamGetFormat(_s);
		public int GetFrames() => HandleError(XtStreamGetFrames(_s, out var r), r);
		public XtLatency GetLatency() => HandleError(XtStreamGetLatency(_s, out var r), r);
	}
}