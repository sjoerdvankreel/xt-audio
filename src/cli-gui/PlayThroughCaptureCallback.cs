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

        internal override void OnCallback(XtFormat format, bool interleaved,
            bool raw, object input, object output, int frames) {

            if (frames == 0)
                return;
            int frameSize = format.inputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            if (!raw && !interleaved)
                Utility.Interleave((Array)input, interleavedBuffer, format.mix.sample, format.inputs, frames);
            if (raw && interleaved)
                Utility.Copy((IntPtr)input, interleavedBuffer, format.mix.sample, format.inputs, frames);
            if (raw && !interleaved)
                Utility.Interleave((IntPtr)input, interleavedBuffer, format.mix.sample, format.inputs, frames);
            if (!buffer.Write(!raw && interleaved ? (Array)input : interleavedBuffer, frames * frameSize))
                onError("Error: circular buffer is full.");
        }
    }
}
