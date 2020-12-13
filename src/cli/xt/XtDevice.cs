using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using static Xt.Utility;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public sealed class XtDevice : IDisposable
    {
        [DllImport("xt-core")] static extern void XtDeviceDestroy(IntPtr d);
        [DllImport("xt-core")] static extern ulong XtDeviceShowControlPanel(IntPtr d);
        [DllImport("xt-core")] static extern ulong XtDeviceGetMix(IntPtr d, out bool valid, out XtMix mix);
        [DllImport("xt-core")] static extern ulong XtDeviceGetChannelCount(IntPtr d, bool output, out int count);
        [DllImport("xt-core")] static extern ulong XtDeviceSupportsAccess(IntPtr d, bool interleaved, out bool supports);
        [DllImport("xt-core")] static extern ulong XtDeviceSupportsFormat(IntPtr d, in XtFormat format, out bool supports);
        [DllImport("xt-core")] static extern ulong XtDeviceGetBufferSize(IntPtr d, in XtFormat format, out XtBufferSize size);
        [DllImport("xt-core")] static extern ulong XtDeviceGetChannelName(IntPtr d, bool output, int index, [Out] byte[] buffer, ref int size);
        [DllImport("xt-core")] static extern ulong XtDeviceOpenStream(IntPtr d, in DeviceStreamParams @params, IntPtr user, out IntPtr stream);

        readonly IntPtr _d;
        internal IntPtr Handle() => _d;
        internal XtDevice(IntPtr d) => _d = d;

        public void Dispose() => XtDeviceDestroy(_d);
        public void ShowControlPanel() => HandleError(XtDeviceShowControlPanel(_d));
        public XtMix? GetMix() => HandleError(XtDeviceGetMix(_d, out var v, out var r)) && v ? r : (XtMix?)null;
        public int GetChannelCount(bool output) => HandleError(XtDeviceGetChannelCount(_d, output, out var r), r);
        public bool SupportsFormat(in XtFormat format) => HandleError(XtDeviceSupportsFormat(_d, in format, out var r), r);
        public bool SupportsAccess(bool interleaved) => HandleError(XtDeviceSupportsAccess(_d, interleaved, out var r), r);
        public XtBufferSize GetBufferSize(in XtFormat format) => HandleError(XtDeviceGetBufferSize(_d, in format, out var r), r);

        public string GetChannelName(bool output, int index)
        {
            int size = 0;
            HandleError(XtDeviceGetChannelName(_d, output, index, null, ref size));
            var buffer = new byte[size];
            HandleError(XtDeviceGetChannelName(_d, output, index, buffer, ref size));
            return Encoding.UTF8.GetString(buffer, 0, size - 1);
        }

        public XtStream OpenStream(in XtDeviceStreamParams @params, object user)
        {
            var result = new XtStream(@params.stream.onBuffer, @params.stream.onXRun, user);
            var native = new DeviceStreamParams();
            native.format = @params.format;
            native.bufferSize = @params.bufferSize;
            native.stream.onBuffer = result.OnNativeBuffer();
            native.stream.interleaved = @params.stream.interleaved ? 1 : 0;
            native.stream.onXRun = @params.stream.onXRun == null ? null : result.OnNativeXRun();
            result.Init(HandleError(XtDeviceOpenStream(_d, @native, IntPtr.Zero, out var r), r));
            return result;
        }
    }
}