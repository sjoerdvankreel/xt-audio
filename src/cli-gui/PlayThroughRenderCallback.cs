using System;

namespace Xt {

    class PlayThroughRenderCallback : StreamCallback {

        private readonly ConcurrentRingBuffer buffer;

        internal PlayThroughRenderCallback(Action<string> onError,
            Action<string> onMessage, ConcurrentRingBuffer buffer) :
            base("PlayThroughRender", onError, onMessage) {
            this.buffer = buffer;
        }

        internal override void OnMessage(string message) {
            base.OnMessage(message);
            double fill = (double)buffer.Full() / ConcurrentRingBuffer.Size;
            base.OnMessage(string.Format("Circular buffer fill factor: {0}.", fill));
        }

        internal override void OnCallback(XtFormat format, Array input, Array output, int frames) {

            if (frames == 0)
                return;
            int frameSize = format.outputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            if (!buffer.Read(output, frames * frameSize)) {
                onMessage.Invoke("Warning: circular buffer is empty.");
                Array.Clear(output, 0, frames);
            }
        }
    }
}
