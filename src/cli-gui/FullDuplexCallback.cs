using System;

namespace Xt {

    class FullDuplexCallback : StreamCallback {

        internal FullDuplexCallback(Action<string> onError, Action<string> onMessage) :
            base("FullDuplex", onError, onMessage) {
        }

        internal override void OnCallback(XtFormat format, Array input, Array output, int frames) {
            int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
            Buffer.BlockCopy(input, 0, output, 0, frames * format.inputs * sampleSize);
        }
    }
}
