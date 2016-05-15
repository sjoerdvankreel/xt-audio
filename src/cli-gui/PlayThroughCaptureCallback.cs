using System;

namespace Xt {

    class PlayThroughCaptureCallback : StreamCallback {

        private readonly ConcurrentRingBuffer buffer;

        internal PlayThroughCaptureCallback(Action<string> onError,
            Action<string> onMessage, ConcurrentRingBuffer buffer) :
            base("PlayThroughCapture", onError, onMessage) {
            this.buffer = buffer;
        }

        internal override void OnCallback(XtFormat format, Array input, Array output, int frames) {

            if (frames == 0)
                return;
            int frameSize = format.inputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            if (!buffer.Write(input, frames * frameSize))
                onError("Error: circular buffer is full.");
        }
    }
}
