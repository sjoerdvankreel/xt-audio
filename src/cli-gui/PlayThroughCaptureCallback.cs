using System;

namespace Xt {

    class PlayThroughCaptureCallback : StreamCallback {

        private readonly int frameSize;
        private readonly ConcurrentRingBuffer buffer;

        internal PlayThroughCaptureCallback(XtFormat format, Action<string> onError,
            Action<string> onMessage, ConcurrentRingBuffer buffer) :
            base("PlayThroughCapture", format, onError, onMessage) {
            this.buffer = buffer;
            this.frameSize = format.inputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
        }

        internal override void OnCallback(Array input, Array output, int frames) {

            if (frames > 0)
                if (!buffer.Write(input, frames * frameSize))
                    onError("Error: circular buffer is full.");
        }
    }
}
