using System;

namespace Xt {

    class PlayThroughRenderCallback : StreamCallback {

        private Array interleavedBuffer;
        private readonly ConcurrentRingBuffer buffer;

        internal PlayThroughRenderCallback(Action<string> onError,
            Action<string> onMessage, ConcurrentRingBuffer buffer) :
            base("PlayThroughRender", onError, onMessage) {
            this.buffer = buffer;
        }

        internal void Init(XtFormat format, int frames) {
            interleavedBuffer = Utility.CreateInterleavedBuffer(format.mix.sample, format.outputs, frames);
        }

        internal override void OnMessage(string message) {
            base.OnMessage(message);
            double fill = (double)buffer.Full() / ConcurrentRingBuffer.Size;
            base.OnMessage(string.Format("Circular buffer fill factor: {0}.", fill));
        }

        internal override void OnCallback(XtFormat format, bool interleaved,
            bool raw, object input, object output, int frames) {

            if (frames == 0)
                return;
            Array outData = !raw && interleaved ? (Array)output : interleavedBuffer;
            int frameSize = format.outputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            if (!buffer.Read(outData, frames * frameSize)) {
                onMessage.Invoke("Warning: circular buffer is empty.");
                Array.Clear(outData, 0, frames * (format.mix.sample == XtSample.Int24 ? 3 : 1));
            }
            if (!raw && !interleaved)
                Utility.Deinterleave(interleavedBuffer, (Array)output, format.mix.sample, format.outputs, frames);
            if (raw && interleaved)
                Utility.Copy(interleavedBuffer, (IntPtr)output, format.mix.sample, format.outputs, frames);
            if(raw && !interleaved)
                Utility.Deinterleave(interleavedBuffer, (IntPtr)output, format.mix.sample, format.outputs, frames);
        }
    }
}
