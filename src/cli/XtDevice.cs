using System;
using System.Text;
using static Xt.XtNative;

namespace Xt
{
	public sealed class XtDevice : IDisposable
	{
		internal IntPtr _handle;
		internal XtDevice(IntPtr handle) 
			=> _handle = handle;

		public override string ToString()
			=> GetName();
		public void ShowControlPanel()
			=> HandleError(XtDeviceShowControlPanel(_handle));
		public XtBufferSize GetBufferSize(in XtFormat format)
			=> HandleError(XtDeviceGetBufferSize(_handle, in format, out var result)) ? result : default;
		public int GetChannelCount(bool output)
			=> HandleError(XtDeviceGetChannelCount(_handle, output, out var result)) ? result : default;
		public bool SupportsFormat(in XtFormat format)
			=> HandleError(XtDeviceSupportsFormat(_handle, in format, out var result)) ? result : default;
		public bool SupportsAccess(bool interleaved)
			=> HandleError(XtDeviceSupportsAccess(_handle, interleaved, out var result)) ? result : default;
		public XtMix? GetMix()
			=> HandleError(XtDeviceGetMix(_handle, out var valid, out var result)) && valid ? result : default;

		public void Dispose()
		{
			XtDeviceDestroy(_handle);
			_handle = IntPtr.Zero;
		}

		public string GetName()
		{
			int size = 0;
			HandleError(XtDeviceGetName(_handle, null, ref size));
			var buffer = new byte[size];
			HandleError(XtDeviceGetName(_handle, buffer, ref size));
			return Encoding.UTF8.GetString(buffer, 0, size - 1);
		}

		public string GetChannelName(bool output, int index)
		{
			int size = 0;
			HandleError(XtDeviceGetChannelName(_handle, output, index, null, ref size));
			var buffer = new byte[size];
			HandleError(XtDeviceGetChannelName(_handle, output, index, buffer, ref size));
			return Encoding.UTF8.GetString(buffer, 0, size - 1);
		}

		public XtStream OpenStream(in XtFormat format, bool interleaved, bool raw, double bufferSize, XtStreamCallback streamCallback, XtXRunCallback xRunCallback, object user)
		{
			XtStream result = new XtStream(interleaved, raw, streamCallback, xRunCallback, user);
			HandleError(XtDeviceOpenStream(_handle, in format, interleaved, bufferSize, result.streamCallbackPtr, result.xRunCallbackPtr, IntPtr.Zero, out var stream));
			result.Init(stream);
			return result;
		}
	}
}