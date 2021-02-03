using System;
using System.IO;

namespace Xt
{
    public class OnCapture : OnBuffer
    {
        byte[] _block;
        int _frameSize;
        Array _interleavedBuffer;
        readonly FileStream _file;

        public OnCapture(OnBufferParams @params, FileStream file) : base(@params) => _file = file;

        public void Init(in XtFormat format, int maxFrames)
        {
            var attrs = XtAudio.GetSampleAttributes(format.mix.sample);
            _frameSize = format.channels.inputs * attrs.size;
            _block = new byte[maxFrames * _frameSize];
            _interleavedBuffer = Utility.CreateInterleavedBuffer(format.mix.sample, format.channels.inputs, maxFrames);
        }

        protected override void ProcessBuffer(XtStream stream, in XtBuffer buffer, XtSafeBuffer safe)
        {
            var frames = buffer.frames;
            var format = stream.GetFormat();
            var sample = format.mix.sample;
            var channels = format.channels.inputs;
            if (buffer.frames == 0) return;

            if (!Params.Native && !Params.Interleaved)
                Utility.Interleave(safe.GetInput(), _interleavedBuffer, sample, channels, frames);
            else if (Params.Native && !Params.Interleaved)
                Utility.Interleave(buffer.input, _interleavedBuffer, sample, channels, frames);
            else if (Params.Native && Params.Interleaved)
                Utility.Copy(buffer.input, _interleavedBuffer, format.mix.sample, format.channels.inputs, frames);
            else
                Buffer.BlockCopy(safe.GetInput(), 0, _interleavedBuffer, 0, frames * _frameSize);
            Buffer.BlockCopy(_interleavedBuffer, 0, _block, 0, frames * _frameSize);
            _file.Write(_block, 0, frames * _frameSize);
        }
    }
}