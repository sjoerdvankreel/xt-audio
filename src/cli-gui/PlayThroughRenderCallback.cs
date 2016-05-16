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

        internal override void OnCallback(XtFormat format, bool interleaved, Array input, Array output, int frames) {

            if (frames == 0)
                return;
            Array outData = interleaved ? output : interleavedBuffer;
            int frameSize = format.outputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            if (!buffer.Read(outData, frames * frameSize)) {
                onMessage.Invoke("Warning: circular buffer is empty.");
                Array.Clear(outData, 0, frames * (format.mix.sample == XtSample.Int24 ? 3 : 1));
            }
            if (!interleaved)
                Utility.Deinterleave(interleavedBuffer, output, format.mix.sample, format.outputs, frames);
        }
    }
}
