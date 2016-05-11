using System;
using System.Collections.Concurrent;

namespace Xt {

    class FullDuplexCallback : StreamCallback {

        readonly int sampleSize;

        internal FullDuplexCallback(XtFormat format, 
            Action<string> onError, Action<string> onMessage) :
            base("FullDuplex", format, onError, onMessage) {
            sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
        }

        internal override void OnCallback(Array input, Array output, int frames) {
            Buffer.BlockCopy(input, 0, output, 0, frames * format.inputs * sampleSize);
        }
    }
}
