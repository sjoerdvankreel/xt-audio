using System;
using System.Linq;
using System.Runtime.InteropServices;
using static Xt.XtNative;

namespace Xt
{
    public sealed class XtService
    {
        readonly IntPtr _handle;
        internal XtService(IntPtr handle) 
            => _handle = handle;

        public override string ToString()
            => GetName();
		public XtSystem GetSystem() 
            => XtServiceGetSystem(_handle);
        public XtCapabilities GetCapabilities() 
            => XtServiceGetCapabilities(_handle);
        public string GetName() 
            => NativeUtility.PtrToStringUTF8(XtServiceGetName(_handle));
        public int GetDeviceCount()
            => HandleError(XtServiceGetDeviceCount(_handle, out var result))? result: default;
        public XtDevice OpenDevice(int index)
            => HandleError(XtServiceOpenDevice(_handle, index, out var result))? new XtDevice(result): default;
        public XtDevice OpenDefaultDevice(bool output)
            => HandleError(XtServiceOpenDefaultDevice(_handle, output, out var result)) ? new XtDevice(result) : default;

        public unsafe XtStream AggregateStream(XtDevice[] devices, XtChannels[] channels,
            double[] bufferSizes, int count, XtMix mix, bool interleaved, bool raw,
            XtDevice master, XtStreamCallback streamCallback, XtXRunCallback xRunCallback, object user)
        {
            IntPtr str;
            IntPtr channelsPtr = IntPtr.Zero;
            IntPtr[] ds = devices.Select(d => d._handle).ToArray();
            XtStream stream = new XtStream(raw, streamCallback, xRunCallback, user);
            try
            {
                int size = Marshal.SizeOf(typeof(XtChannels));
                channelsPtr = Marshal.AllocHGlobal(count * size);
                for (int i = 0; i < count; i++)
                    Marshal.StructureToPtr(channels[i], new IntPtr((byte*)channelsPtr + i * size), false);
                XtNative.HandleError(XtNative.XtServiceAggregateStream(_handle, ds, channelsPtr, bufferSizes, count,
                    mix, interleaved, master._handle, stream.streamCallbackPtr, stream.xRunCallbackPtr, IntPtr.Zero, out str));
            } finally
            {
                if (channelsPtr != IntPtr.Zero)
                    Marshal.FreeHGlobal(channelsPtr);
            }
            stream.Init(str);
            return stream;
        }
    }
}