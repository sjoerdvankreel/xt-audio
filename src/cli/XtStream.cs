using System;
using System.Runtime.InteropServices;
using static Xt.XtNative;

namespace Xt
{
    public sealed class XtStream : IDisposable
    {
        [DllImport("xt-core")] static extern ulong XtStreamStop(IntPtr s);
        [DllImport("xt-core")] static extern ulong XtStreamStart(IntPtr s);
        [DllImport("xt-core")] static extern void XtStreamDestroy(IntPtr s);
        [DllImport("xt-core")] static extern unsafe XtFormat* XtStreamGetFormat(IntPtr s);
        [DllImport("xt-core")] static extern ulong XtStreamGetFrames(IntPtr s, out int frames);
        [DllImport("xt-core")] static extern ulong XtStreamGetLatency(IntPtr s, out XtLatency latency);

        IntPtr _s;
        readonly object _user;
        readonly XtXRunCallback _xRunCallback;
        readonly XtStreamCallback _streamCallback;
        readonly XRunCallback _nativeXRunCallback;
        readonly StreamCallback _nativeStreamCallback;

        internal XtStream(XtStreamCallback streamCallback, XtXRunCallback xRunCallback, object user)
        {
            _user = user;
            _xRunCallback = xRunCallback;
            _streamCallback = streamCallback;
            _nativeXRunCallback = XRunCallback;
            _nativeStreamCallback = StreamCallback;
        }

        public void Dispose() => XtStreamDestroy(_s);
        public void Stop() => HandleError(XtStreamStop(_s));
        public void Start() => HandleError(XtStreamStart(_s));
        public unsafe XtFormat GetFormat() => *XtStreamGetFormat(_s);
        public int GetFrames() => HandleError(XtStreamGetFrames(_s, out var r), r);
        public XtLatency GetLatency() => HandleError(XtStreamGetLatency(_s, out var r), r);

        internal void Init(IntPtr s) => _s = s;
        internal XRunCallback NativeXRunCallback() => _nativeXRunCallback;
        internal StreamCallback NativeStreamCallback() => _nativeStreamCallback;
        void XRunCallback(int index, IntPtr user) => _xRunCallback(index, _user);
        void StreamCallback(IntPtr stream, in XtBuffer buffer, IntPtr user) => _streamCallback(this, in buffer, _user);
    }
}