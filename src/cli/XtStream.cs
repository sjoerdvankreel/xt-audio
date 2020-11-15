using System;
using System.Runtime.InteropServices;
using System.Security;
using static Xt.Utility;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
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
        readonly XtOnXRun _onXRun;
        readonly XtOnBuffer _onBuffer;
        readonly OnXRun _onNativeXRun;
        readonly OnBuffer _onNativeBuffer;

        internal XtStream(XtOnBuffer onBuffer, XtOnXRun onXRun, object user)
        {
            _user = user;
            _onXRun = onXRun;
            _onBuffer = onBuffer;
            _onNativeXRun = OnXRun;
            _onNativeBuffer = OnBuffer;
        }

        public void Dispose() => XtStreamDestroy(_s);
        public void Stop() => HandleError(XtStreamStop(_s));
        public void Start() => HandleError(XtStreamStart(_s));
        public unsafe XtFormat GetFormat() => *XtStreamGetFormat(_s);
        public int GetFrames() => HandleError(XtStreamGetFrames(_s, out var r), r);
        public XtLatency GetLatency() => HandleError(XtStreamGetLatency(_s, out var r), r);

        internal void Init(IntPtr s) => _s = s;
        internal OnXRun OnNativeXRun() => _onNativeXRun;
        internal OnBuffer OnNativeBuffer() => _onNativeBuffer;
        void OnXRun(int index, IntPtr user) => _onXRun(index, _user);
        void OnBuffer(IntPtr stream, in XtBuffer buffer, IntPtr user) => _onBuffer(this, in buffer, _user);
    }
}