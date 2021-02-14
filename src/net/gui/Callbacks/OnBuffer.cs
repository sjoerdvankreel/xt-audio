using System;

namespace Xt
{
    public abstract class OnBuffer
    {
        const string FormatString =
            "Stream {1}:{0}" +
            "\tinput latency:{2}{0}" +
            "\toutput latency:{3}{0}" +
            "\tbuffer frames:{4}{0}" +
            "\tcurrent frames:{5}{0}" +
            "\ttime:{6}{0}" +
            "\tposition:{7}{0}" +
            "\ttimeValid:{8}{0}" +
            "\tuser:{9}.";

        int _processed;
        protected OnBufferParams Params { get; }

        public OnBuffer(OnBufferParams @params) => Params = @params;
        protected abstract void ProcessBuffer(XtStream stream, in XtBuffer buffer, XtSafeBuffer safe);

        public int Callback(XtStream stream, in XtBuffer buffer, object user)
        {
            XtFormat format = stream.GetFormat();
            _processed += buffer.frames;
            if (Params.Native)
                ProcessBuffer(stream, in buffer, null);
            else
            {
                var safe = XtSafeBuffer.Get(stream);
                safe.Lock(in buffer);
                ProcessBuffer(stream, in buffer, safe);
                safe.Unlock(in buffer);
            }
            if (_processed < format.mix.rate * 3) return 0;

            _processed = 0;
            XtLatency latency = stream.GetLatency();
            XtBuffer bufferLocal = buffer;
            Params.OnMessage(() => string.Format(FormatString, Environment.NewLine,
                Params.Name, latency.input, latency.output, stream.GetFrames(), bufferLocal.frames,
                bufferLocal.time, bufferLocal.position, bufferLocal.timeValid, user));
            return 0;
        }
    }
}