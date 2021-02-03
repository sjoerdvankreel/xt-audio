using System;

namespace Xt
{
    public unsafe class OnFullDuplex : OnBuffer
    {
        public OnFullDuplex(OnBufferParams @params) : base(@params) { }

        protected override void ProcessBuffer(XtStream stream, in XtBuffer buffer, XtSafeBuffer safe)
        {
            var frames = buffer.frames;
            bool native = Params.Native;
            var format = stream.GetFormat();
            var inputs = format.channels.inputs;
            bool interleaved = Params.Interleaved;
            int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
            var channelBytes = frames * sampleSize;
            var bufferBytes = frames * inputs * sampleSize;

            if (interleaved && !native)
                Buffer.BlockCopy(safe.GetInput(), 0, safe.GetOutput(), 0, bufferBytes);
            if (interleaved && native)
                Utility.MemCpy(buffer.output, buffer.input, new IntPtr(bufferBytes));
            if (!interleaved && !native)
                for (int i = 0; i < inputs; i++)
                    Buffer.BlockCopy(
                        (Array)safe.GetInput().GetValue(i), 0,
                        (Array)safe.GetOutput().GetValue(i), 0, channelBytes);
            if (!interleaved && native)
                for (int i = 0; i < format.channels.inputs; i++)
                    Utility.MemCpy(
                        new IntPtr(((void**)((buffer.output).ToPointer()))[i]),
                        new IntPtr(((void**)((buffer.input).ToPointer()))[i]),
                        new IntPtr(channelBytes));
        }
    }
}