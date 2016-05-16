using System;
using System.IO;

namespace Xt {

    class CaptureCallback : StreamCallback {

        internal byte[] block;
        internal int frameSize;
        private readonly FileStream stream;

        internal CaptureCallback(Action<string> onError,
            Action<string> onMessage, FileStream stream) :
            base("Capture", onError, onMessage) {
            this.stream = stream;
        }

        internal void Init(XtFormat format, int maxFrames) {
            frameSize = format.inputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            block = new byte[maxFrames * frameSize];
        }

        internal override void OnCallback(XtFormat format, bool interleaved, Array input, Array output, int frames) {

            if (frames == 0)
                return;
            Buffer.BlockCopy(input, 0, block, 0, frames * frameSize);
            stream.Write(block, 0, frames * frameSize);
        }
    }
}
