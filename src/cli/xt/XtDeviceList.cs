using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using static Xt.Utility;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public sealed class XtDeviceList : IDisposable
    {
        [DllImport("xt-audio")] static extern void XtDeviceListDestroy(IntPtr l);
        [DllImport("xt-audio")] static extern ulong XtDeviceListGetCount(IntPtr l, out int count);
        [DllImport("xt-audio")] static extern ulong XtDeviceListGetId(IntPtr l, int index, [Out] byte[] buffer, ref int size);
        [DllImport("xt-audio")] static extern ulong XtDeviceListGetName(IntPtr l, byte[] id, [Out] byte[] buffer, ref int size);
        [DllImport("xt-audio")] static extern ulong XtDeviceListGetCapabilities(IntPtr l, byte[] id, out XtDeviceCaps capabilities);

        IntPtr _l;
        internal XtDeviceList(IntPtr l) => _l = l;

        public void Dispose() { XtDeviceListDestroy(_l); _l = IntPtr.Zero; }
        public int GetCount() => HandleError(XtDeviceListGetCount(_l, out var r), r);

        public string GetId(int index)
        {
            int size = 0;
            HandleError(XtDeviceListGetId(_l, index, null, ref size));
            var buffer = new byte[size];
            HandleError(XtDeviceListGetId(_l, index, buffer, ref size));
            return Encoding.UTF8.GetString(buffer, 0, size - 1);
        }

        public string GetName(string id)
        {
            int size = 0;
            var bytes = Encoding.UTF8.GetBytes(id + char.MinValue);
            HandleError(XtDeviceListGetName(_l, bytes, null, ref size));
            var buffer = new byte[size];
            HandleError(XtDeviceListGetName(_l, bytes, buffer, ref size));
            return Encoding.UTF8.GetString(buffer, 0, size - 1);
        }

        public XtDeviceCaps GetCapabilities(string id)
        {
            var bytes = Encoding.UTF8.GetBytes(id + char.MinValue);
            return HandleError(XtDeviceListGetCapabilities(_l, bytes, out var r), r);
        }
    }
}