using System;
using System.Linq;
using System.Runtime.InteropServices;
using static Xt.XtNative;

namespace Xt
{
	public sealed class XtService
	{
		readonly IntPtr _handle;
		internal XtService(IntPtr handle) => _handle = handle;

		public XtCapabilities GetCapabilities() 
		=> XtServiceGetCapabilities(_handle);
		public int GetDeviceCount() 
		=> HandleError(XtServiceGetDeviceCount(_handle, out var r), r);
		public XtDevice OpenDevice(int index) 
		=> HandleError(XtServiceOpenDevice(_handle, index, out var r), new XtDevice(r));
		public XtDevice OpenDefaultDevice(bool output) 
		=> HandleError(XtServiceOpenDefaultDevice(_handle, output, out var r), new XtDevice(r));

		public unsafe XtStream AggregateStream(XtDevice[] devices, XtChannels[] channels,
			double[] bufferSizes, int count, in XtMix mix, bool interleaved,
			XtDevice master, XtStreamCallback streamCallback, XtXRunCallback xRunCallback, IntPtr user)
		{
			IntPtr str;
			IntPtr channelsPtr = IntPtr.Zero;
			IntPtr[] ds = devices.Select(d => d.Handle).ToArray();
			try
			{
				int size = Marshal.SizeOf(typeof(XtChannels));
				channelsPtr = Marshal.AllocHGlobal(count * size);
				for (int i = 0; i < count; i++)
					Marshal.StructureToPtr(channels[i], new IntPtr((byte*)channelsPtr + i * size), false);
				HandleError(XtServiceAggregateStream(_handle, ds, channelsPtr, bufferSizes, count,
					in mix, interleaved, master.Handle, streamCallback, xRunCallback, user, out str));
			}
			finally
			{
				if (channelsPtr != IntPtr.Zero)
					Marshal.FreeHGlobal(channelsPtr);
			}
			return new XtStream(str, streamCallback, xRunCallback);
		}

		public XtStream AggregateStream(XtDevice[] devices, XtChannels[] channels,
			double[] bufferSizes, int count, XtMix mix, bool interleaved,
			XtDevice master, XtManagedStreamCallback streamCallback, XtManagedXRunCallback xRunCallback, object user)
		{
			var streamWrapper = new ManagedStreamCallback(streamCallback, interleaved, user);
			var xRunWrapper = xRunCallback == null ? (XtXRunCallback)null : new ManagedXRunCallback(xRunCallback, user).Callback;
			var result = AggregateStream(devices, channels, bufferSizes, count, in mix, interleaved, master, streamWrapper.Callback, xRunWrapper, IntPtr.Zero);
			streamWrapper.Init(result);
			return result;
		}
	}
}