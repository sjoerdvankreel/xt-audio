using System;

namespace Xt {

    class PlayThroughCaptureCallback : StreamCallback {

        private Array interleavedBuffer;
        private readonly ConcurrentRingBuffer buffer;

        internal PlayThroughCaptureCallback(Action<string> onError,
            Action<string> onMessage, ConcurrentRingBuffer buffer) :
            base("PlayThroughCapture", onError, onMessage) {
            this.buffer = buffer;
        }

        internal void Init(XtFormat format, int frames) {
            interleavedBuffer = Utility.CreateInterleavedBuffer(format.mix.sample, format.inputs, frames);
        }

        internal override void OnCallback(XtFormat format, bool interleaved, Array input, Array output, int frames) {

            if (frames == 0)
                return;
            Array inData = input;
            int frameSize = format.inputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            if (!interleaved) {
                Utility.Interleave(input, interleavedBuffer, format.mix.sample, format.inputs, frames);
                inData = interleavedBuffer;
            }
            if (!buffer.Write(inData, frames * frameSize))
                onError("Error: circular buffer is full.");
        }
    }
}
