using System;
using System.Runtime.InteropServices;

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
            IntPtr name;
            XtNative.HandleError(XtNative.XtDeviceGetName(d, out name));
            return XtNative.FreeStringFromUtf8(name);
        }

        public int GetChannelCount(bool output)
        {
            int count;
            XtNative.HandleError(XtNative.XtDeviceGetChannelCount(d, output, out count));
            return count;
        }

        public XtBuffer GetBuffer(XtFormat format)
        {
            XtBuffer buffer;
            XtNative.Format native = XtNative.Format.ToNative(format);
            XtNative.HandleError(XtNative.XtDeviceGetBuffer(d, ref native, out buffer));
            return buffer;
        }

        public XtMix GetMix()
        {
            IntPtr mix;
            XtNative.HandleError(XtNative.XtDeviceGetMix(d, out mix));
            XtMix result = mix == IntPtr.Zero ? null : (XtMix)Marshal.PtrToStructure(mix, typeof(XtMix));
            XtNative.XtAudioFree(mix);
            return result;
        }

        public bool SupportsAccess(bool interleaved)
        {
            bool supports;
            XtNative.HandleError(XtNative.XtDeviceSupportsAccess(d, interleaved, out supports));
            return supports;
        }

        public bool SupportsFormat(XtFormat format)
        {
            bool supports;
            XtNative.Format native = XtNative.Format.ToNative(format);
            XtNative.HandleError(XtNative.XtDeviceSupportsFormat(d, ref native, out supports));
            return supports;
        }

        public string GetChannelName(bool output, int index)
        {
            IntPtr name;
            XtNative.HandleError(XtNative.XtDeviceGetChannelName(d, output, index, out name));
            return XtNative.FreeStringFromUtf8(name);
        }

        public XtStream OpenStream(XtFormat format, bool interleaved, bool raw, double bufferSize,
            XtStreamCallback streamCallback, XtXRunCallback xRunCallback, object user)
        {
            IntPtr s;
            XtStream stream = new XtStream(raw, streamCallback, xRunCallback, user);
            XtNative.Format native = XtNative.Format.ToNative(format);
            XtNative.HandleError(XtNative.XtDeviceOpenStream(d, ref native, interleaved,
                bufferSize, stream.streamCallbackPtr, stream.xRunCallbackPtr, IntPtr.Zero, out s));
            stream.Init(s);
            return stream;
        }
    }
}