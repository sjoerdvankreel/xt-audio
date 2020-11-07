using System;
using System.Text;
using static Xt.XtNative;

namespace Xt
{
	public sealed class XtDevice : IDisposable
	{
		readonly IntPtr _d;
		internal XtDevice(IntPtr d) => _d = d;

		public void Dispose() => XtDeviceDestroy(_d);
		public override string ToString() => GetName();
		public void ShowControlPanel() => HandleError(XtDeviceShowControlPanel(_d));
		public XtMix? GetMix() => HandleError(XtDeviceGetMix(_d, out var v, out var r)) && v ? r : default;
		public int GetChannelCount(bool output) => HandleError(XtDeviceGetChannelCount(_d, output, out var r), r);
		public bool SupportsFormat(in XtFormat format) => HandleError(XtDeviceSupportsFormat(_d, in format, out var r), r);
		public bool SupportsAccess(bool interleaved) => HandleError(XtDeviceSupportsAccess(_d, interleaved, out var r), r);
		public XtBufferSize GetBufferSize(in XtFormat format) => HandleError(XtDeviceGetBufferSize(_d, in format, out var r), r);
		public XtStream OpenStream(in XtFormat format, bool interleaved, double bufferSize, XtStreamCallback streamCallback, XtXRunCallback xRunCallback, IntPtr user)
			=> HandleError(XtDeviceOpenStream(_d, in format, interleaved, bufferSize, streamCallback, xRunCallback, user, out IntPtr s), new XtStream(s, streamCallback, xRunCallback));

		public string GetName()
		{
			int size = 0;
			HandleError(XtDeviceGetName(_d, null, ref size));
			var buffer = new byte[size];
			HandleError(XtDeviceGetName(_d, buffer, ref size));
			return Encoding.UTF8.GetString(buffer, 0, size - 1);
		}

		public string GetChannelName(bool output, int index)
		{
			int size = 0;
			HandleError(XtDeviceGetChannelName(_d, output, index, null, ref size));
			var buffer = new byte[size];
			HandleError(XtDeviceGetChannelName(_d, output, index, buffer, ref size));
			return Encoding.UTF8.GetString(buffer, 0, size - 1);
		}

		public XtStream OpenStream(in XtFormat format, bool interleaved, double bufferSize, XtManagedStreamCallback streamCallback, XtXRunCallback xRunCallback)
		{
			var streamWrapper = new StreamCallbackWrapper(in format, interleaved, streamCallback);
			var result = OpenStream(in format, interleaved, bufferSize, streamWrapper.Callback, xRunCallback, IntPtr.Zero);
			streamWrapper._stream = result;
			streamWrapper._frames = result.GetFrames();
			return result;
		}
	}
}