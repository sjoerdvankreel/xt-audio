using System;
using System.Runtime.InteropServices;
using System.Security;
using static Xt.Utility;

namespace Xt
{
    [SuppressUnmanagedCodeSecurity]
    public sealed class XtStream : IDisposable
    {
        [DllImport("xt-audio")] 
        static extern void XtStreamStop(IntPtr s);
        [DllImport("xt-audio")] 
        static extern ulong XtStreamStart(IntPtr s);
        [DllImport("xt-audio")] 
        static extern void XtStreamDestroy(IntPtr s);
        [DllImport("xt-audio")] 
        static extern int XtStreamIsRunning(IntPtr s);
        [DllImport("xt-audio")]
        static extern IntPtr XtStreamGetHandle(IntPtr s);
        [DllImport("xt-audio")]
        static extern int XtStreamIsInterleaved(IntPtr s);
        [DllImport("xt-audio")]
        static extern unsafe XtFormat* XtStreamGetFormat(IntPtr s);
        [DllImport("xt-audio")]
        static extern ulong XtStreamGetFrames(IntPtr s, out int frames);
        [DllImport("xt-audio")]
        static extern ulong XtStreamGetLatency(IntPtr s, out XtLatency latency);

        IntPtr _s;
        readonly object _user;
        readonly XtStreamParams _params;
        readonly OnXRun _onNativeXRun;
        readonly OnBuffer _onNativeBuffer;
        readonly OnRunning _onNativeRunning;

        internal XtStream(in XtStreamParams @params, object user)
        {
            _user = user;
            _params = @params;
            _onNativeXRun = OnXRun;
            _onNativeBuffer = OnBuffer;
            _onNativeRunning = OnRunning;
        }

        internal void Init(IntPtr s) => _s = s;
        internal OnXRun OnNativeXRun() => _onNativeXRun;
        internal OnBuffer OnNativeBuffer() => _onNativeBuffer;
        internal OnRunning OnNativeRunning() => _onNativeRunning;

        void OnXRun(IntPtr stream, int index, IntPtr user) 
        => _params.onXRun(this, index, _user);
        int OnBuffer(IntPtr stream, in XtBuffer buffer, IntPtr user)
        => _params.onBuffer(this, in buffer, _user);
        void OnRunning(IntPtr stream, int running, ulong error, IntPtr user)
        => _params.onRunning(this, running != 0, error, _user);

        public void Start() => HandleError(XtStreamStart(_s));
        public void Stop() => HandleAssert(() => XtStreamStop(_s));
        public IntPtr GetHandle() => HandleAssert(XtStreamGetHandle(_s));
        public bool IsRunning() => HandleAssert(XtStreamIsRunning(_s) != 0);
        public unsafe XtFormat GetFormat() => HandleAssert(*XtStreamGetFormat(_s));
        public int GetFrames() => HandleError(XtStreamGetFrames(_s, out var r), r);
        public bool IsInterleaved() => HandleAssert(XtStreamIsInterleaved(_s) != 0);
        public XtLatency GetLatency() => HandleError(XtStreamGetLatency(_s, out var r), r);
        public void Dispose() { HandleAssert(() => XtStreamDestroy(_s)); _s = IntPtr.Zero; }
    }
}