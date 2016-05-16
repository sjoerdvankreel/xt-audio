using System;
using System.IO;

namespace Xt {

    class CaptureCallback : StreamCallback {

        private byte[] block;
        private int frameSize;
        private Array interleavedBuffer;
        private readonly FileStream stream;

        internal CaptureCallback(Action<string> onError,
            Action<string> onMessage, FileStream stream) :
            base("Capture", onError, onMessage) {
            this.stream = stream;
        }

        internal void Init(XtFormat format, int maxFrames) {
            frameSize = format.inputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            block = new byte[maxFrames * frameSize];
            interleavedBuffer = Utility.CreateInterleavedBuffer(format.mix.sample, format.inputs, maxFrames);
        }

        internal override void OnCallback(XtFormat format, bool interleaved, Array input, Array output, int frames) {

            if (frames == 0)
                return;
            if (!interleaved)
                Utility.Interleave(input, interleavedBuffer, format.mix.sample, format.inputs, frames);
            Buffer.BlockCopy(interleaved ? input : interleavedBuffer, 0, block, 0, frames * frameSize);
            stream.Write(block, 0, frames * frameSize);
        }
    }
}
