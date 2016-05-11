using System;
using System.IO;

namespace Xt {

    class CaptureCallback : StreamCallback {

        internal byte[] block;
        internal int frameSize;
        private readonly FileStream stream;

        internal CaptureCallback(XtFormat format, Action<string> onError, 
            Action<string> onMessage, FileStream stream) :
            base("Capture", format, onError, onMessage) {
            this.stream = stream;
        }

        internal void Init(int maxFrames) {
            frameSize = format.inputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            block = new byte[maxFrames * frameSize];
        }

        internal override void OnCallback(Array input, Array output, int frames) {

            if (frames > 0) {
                Buffer.BlockCopy(input, 0, block, 0, frames * frameSize);
                stream.Write(block, 0, frames * frameSize);
            }
        }
    }
}
