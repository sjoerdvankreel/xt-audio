using System;
using System.IO;

namespace Xt
{
    class CaptureCallback : StreamCallback
    {
        private byte[] block;
        private int frameSize;
        private Array interleavedBuffer;
        private readonly FileStream stream;

        internal CaptureCallback(Action<Func<string>> onError,
            Action<Func<string>> onMessage, FileStream stream) :
            base("Capture", onError, onMessage)
        {
            this.stream = stream;
        }

        internal void Init(XtFormat format, int maxFrames)
        {
            frameSize = format.inputs * XtAudio.GetSampleAttributes(format.mix.sample).size;
            block = new byte[maxFrames * frameSize];
            interleavedBuffer = Utility.CreateInterleavedBuffer(format.mix.sample, format.inputs, maxFrames);
        }

        internal override void OnCallback(XtFormat format, bool interleaved,
            bool raw, object input, object output, int frames)
        {
            if (frames == 0)
                return;
            if (!raw && !interleaved)
                Utility.Interleave((Array)input, interleavedBuffer, format.mix.sample, format.inputs, frames);
            else if (raw && !interleaved)
                Utility.Interleave((IntPtr)input, interleavedBuffer, format.mix.sample, format.inputs, frames);
            else if (raw && interleaved)
                Utility.Copy((IntPtr)input, interleavedBuffer, format.mix.sample, format.inputs, frames);
            Buffer.BlockCopy(interleaved && !raw ? (Array)input : interleavedBuffer, 0, block, 0, frames * frameSize);
            stream.Write(block, 0, frames * frameSize);
        }
    }
}