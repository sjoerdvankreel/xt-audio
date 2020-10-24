using System;
using System.Text;

namespace Xt
{
	public sealed class XtDevice : IDisposable
	{
		internal IntPtr d;
		internal XtDevice(IntPtr d) => this.d = d;

		public override string ToString() => GetName();
		public XtSystem GetSystem() => XtNative.XtDeviceGetSystem(d);
		public void ShowControlPanel() => XtNative.HandleError(XtNative.XtDeviceShowControlPanel(d));

		public void Dispose()
		{
			if (d != IntPtr.Zero)
				XtNative.XtDeviceDestroy(d);
			d = IntPtr.Zero;
		}

		public string GetName()
		{
			int size = 0;
			XtNative.HandleError(XtNative.XtDeviceGetName(d, null, ref size));
			var buffer = new byte[size];
			XtNative.HandleError(XtNative.XtDeviceGetName(d, buffer, ref size));
			return Encoding.UTF8.GetString(buffer, 0, size - 1);
		}

		public int GetChannelCount(bool output)
		{
			int count;
			XtNative.HandleError(XtNative.XtDeviceGetChannelCount(d, output, out count));
			return count;
		}

		public XtBuffer GetBuffer(in XtFormat format)
		{
			XtNative.HandleError(XtNative.XtDeviceGetBuffer(d, in format, out var result));
			return result;
		}

		public XtMix? GetMix()
		{
			XtMix mix;
			bool valid;
			XtNative.HandleError(XtNative.XtDeviceGetMix(d, out valid, out mix));
			return valid ? mix : new XtMix?();
		}

		public bool SupportsAccess(bool interleaved)
		{
			XtNative.HandleError(XtNative.XtDeviceSupportsAccess(d, interleaved, out var result));
			return result;
		}

		public bool SupportsFormat(in XtFormat format)
		{
			XtNative.HandleError(XtNative.XtDeviceSupportsFormat(d, in format, out var result));
			return result;
		}

		public string GetChannelName(bool output, int index)
		{
			int size = 0;
			XtNative.HandleError(XtNative.XtDeviceGetChannelName(d, output, index, null, ref size));
			var buffer = new byte[size];
			XtNative.HandleError(XtNative.XtDeviceGetChannelName(d, output, index, buffer, ref size));
			return Encoding.UTF8.GetString(buffer, 0, size - 1);
		}

		public XtStream OpenStream(in XtFormat format, bool interleaved, bool raw, double bufferSize,
			XtStreamCallback streamCallback, XtXRunCallback xRunCallback, object user)
		{
			XtStream result = new XtStream(raw, streamCallback, xRunCallback, user);
			XtNative.HandleError(XtNative.XtDeviceOpenStream(d, in format, interleaved,
				bufferSize, result.streamCallbackPtr, result.xRunCallbackPtr, IntPtr.Zero, out var stream));
			result.Init(stream);
			return result;
		}
	}
}