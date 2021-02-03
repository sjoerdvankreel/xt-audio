using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using static Xt.Utility;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public sealed class XtService
    {
        [DllImport("xt-audio")] static extern XtServiceCaps XtServiceGetCapabilities(IntPtr s);
        [DllImport("xt-audio")] static extern ulong XtServiceOpenDevice(IntPtr s, byte[] id, out IntPtr device);
        [DllImport("xt-audio")] static extern ulong XtServiceOpenDeviceList(IntPtr s, XtEnumFlags flags, out IntPtr list);
        [DllImport("xt-audio")] static extern ulong XtServiceAggregateStream(IntPtr s, in AggregateStreamParams @params, IntPtr user, out IntPtr stream);
        [DllImport("xt-audio")] static extern ulong XtServiceGetDefaultDeviceId(IntPtr s, bool output, out bool valid, [Out] byte[] buffer, ref int size);

        readonly IntPtr _s;
        internal XtService(IntPtr s) => _s = s;

        public XtServiceCaps GetCapabilities() => XtServiceGetCapabilities(_s);
        public XtDeviceList OpenDeviceList(XtEnumFlags flags) => HandleError(XtServiceOpenDeviceList(_s, flags, out var r), new XtDeviceList(r));

        static AggregateDeviceParams ToNative(XtAggregateDeviceParams managed)
        {
            var result = new AggregateDeviceParams();
            result.channels = managed.channels;
            result.bufferSize = managed.bufferSize;
            result.device = managed.device.Handle();
            return result;
        }

        public XtDevice OpenDevice(string id)
        {
            byte[] idBytes = Encoding.UTF8.GetBytes(id + char.MinValue);
            return HandleError(XtServiceOpenDevice(_s, idBytes, out var r), new XtDevice(r));
        }

        public string GetDefaultDeviceId(bool output)
        {
            bool valid;
            int size = 0;
            HandleError(XtServiceGetDefaultDeviceId(_s, output, out valid, null, ref size));
            if (!valid) return null;
            var buffer = new byte[size];
            HandleError(XtServiceGetDefaultDeviceId(_s, output, out valid, buffer, ref size));
            if (!valid) return null;
            return Encoding.UTF8.GetString(buffer, 0, size - 1);
        }

        public unsafe XtStream AggregateStream(in XtAggregateStreamParams @params, object user)
        {
            var result = new XtStream(in @params.stream, user);
            var native = new AggregateStreamParams();
            var devices = @params.devices.Select(ToNative).ToArray();
            fixed (AggregateDeviceParams* devs = devices)
            {
                native.mix = @params.mix;
                native.count = @params.count;
                native.devices = new IntPtr(devs);
                native.master = @params.master.Handle();
                native.stream.onBuffer = result.OnNativeBuffer();
                native.stream.interleaved = @params.stream.interleaved ? 1 : 0;
                native.stream.onXRun = @params.stream.onXRun == null ? null : result.OnNativeXRun();
                native.stream.onRunning = @params.stream.onRunning == null ? null : result.OnNativeRunning();
                result.Init(HandleError(XtServiceAggregateStream(_s, in native, IntPtr.Zero, out var r), r));
                return result;
            }
        }
    }
}