using System;
using System.Runtime.InteropServices;

namespace Xt {

    class FullDuplexCallback : StreamCallback {

        internal FullDuplexCallback(Action<string> onError, Action<string> onMessage) :
            base("FullDuplex", onError, onMessage) {
        }

        internal override unsafe void OnCallback(XtFormat format, bool interleaved,
             bool raw, object input, object output, int frames) {

            int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
            if (!raw) {
                if (interleaved)
                    Buffer.BlockCopy((Array)input, 0, (Array)output, 0, frames * format.inputs * sampleSize);
                else
                    for (int i = 0; i < format.inputs; i++)
                        Buffer.BlockCopy((Array)(((Array)input).GetValue(i)), 0, (Array)(((Array)output).GetValue(i)), 0, frames * sampleSize);
            } else {
                if (interleaved)
                    Utility.MemCpy((IntPtr)output, (IntPtr)input, new IntPtr(frames * format.inputs * sampleSize));
                else
                    for (int i = 0; i < format.inputs; i++)
                        Utility.MemCpy(
                            new IntPtr(((void**)(((IntPtr)output).ToPointer()))[i]),
                            new IntPtr(((void**)(((IntPtr)input).ToPointer()))[i]),
                            new IntPtr(frames * sampleSize));
            }
        }
    }
}
