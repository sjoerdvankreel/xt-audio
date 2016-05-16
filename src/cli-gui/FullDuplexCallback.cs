using System;

namespace Xt {

    class FullDuplexCallback : StreamCallback {

        internal FullDuplexCallback(Action<string> onError, Action<string> onMessage) :
            base("FullDuplex", onError, onMessage) {
        }

        internal override void OnCallback(XtFormat format, bool interleaved, Array input, Array output, int frames) {
            int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
            if (interleaved)
                Buffer.BlockCopy(input, 0, output, 0, frames * format.inputs * sampleSize);
            else
                for (int i = 0; i < format.inputs; i++)
                    Buffer.BlockCopy((Array)input.GetValue(i), 0, (Array)output.GetValue(i), 0, frames * sampleSize);
        }
    }
}
