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
        [DllImport("xt-audio")] static extern void XtDeviceDestroy(IntPtr d);
        [DllImport("xt-audio")] static extern IntPtr XtDeviceGetHandle(IntPtr d);
        [DllImport("xt-audio")] static extern ulong XtDeviceShowControlPanel(IntPtr d);
        [DllImport("xt-audio")] static extern ulong XtDeviceGetMix(IntPtr d, out bool valid, out XtMix mix);
        [DllImport("xt-audio")] static extern ulong XtDeviceGetChannelCount(IntPtr d, bool output, out int count);
        [DllImport("xt-audio")] static extern ulong XtDeviceSupportsAccess(IntPtr d, bool interleaved, out bool supports);
        [DllImport("xt-audio")] static extern ulong XtDeviceSupportsFormat(IntPtr d, in XtFormat format, out bool supports);
        [DllImport("xt-audio")] static extern ulong XtDeviceGetBufferSize(IntPtr d, in XtFormat format, out XtBufferSize size);
        [DllImport("xt-audio")] static extern ulong XtDeviceGetChannelName(IntPtr d, bool output, int index, [Out] byte[] buffer, ref int size);
        [DllImport("xt-audio")] static extern ulong XtDeviceOpenStream(IntPtr d, in DeviceStreamParams @params, IntPtr user, out IntPtr stream);

        IntPtr _d;
        internal IntPtr Handle() => _d;
        internal XtDevice(IntPtr d) => _d = d;

        public IntPtr GetHandle() => HandleAssert(XtDeviceGetHandle(_d));
        public void ShowControlPanel() => HandleError(XtDeviceShowControlPanel(_d));
        public void Dispose() { HandleAssert(() => XtDeviceDestroy(_d)); _d = IntPtr.Zero; }
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
            var result = new XtStream(in @params.stream, user);
            var native = new DeviceStreamParams();
            native.format = @params.format;
            native.bufferSize = @params.bufferSize;
            native.stream.onBuffer = result.OnNativeBuffer();
            native.stream.interleaved = @params.stream.interleaved ? 1 : 0;
            native.stream.onXRun = @params.stream.onXRun == null ? null : result.OnNativeXRun();
            native.stream.onRunning = @params.stream.onRunning == null ? null : result.OnNativeRunning();
            result.Init(HandleError(XtDeviceOpenStream(_d, @native, IntPtr.Zero, out var r), r));
            return result;
        }
    }
}