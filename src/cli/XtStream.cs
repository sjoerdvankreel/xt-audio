using System;
using static Xt.XtNative;

namespace Xt
{
	public class XtStream : IDisposable
	{
		IntPtr _handle;
		readonly XtXRunCallback _xRunCallback;
		readonly XtStreamCallback _streamCallback;
		internal XtStream(IntPtr handle, XtStreamCallback streamCallback, XtXRunCallback xRunCallback)
			=> (_handle, _xRunCallback, _streamCallback) = (handle, xRunCallback, streamCallback);

		public void Stop() 
			=> HandleError(XtStreamStop(_handle));
		public void Start() 
			=> HandleError(XtStreamStart(_handle));
		public unsafe XtFormat GetFormat() 
			=> *XtStreamGetFormat(_handle);
		public int GetFrames() 
			=> HandleError(XtStreamGetFrames(_handle, out var result)) ? result : default;
		public XtLatency GetLatency()
			=> HandleError(XtStreamGetLatency(_handle, out var result)) ? result : default;

		public void Dispose()
		{
			if (_handle != IntPtr.Zero) XtStreamDestroy(_handle);
			_handle = IntPtr.Zero;
		}
	}
}