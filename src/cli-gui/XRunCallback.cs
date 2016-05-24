using System;

namespace Xt {

    class XRunCallback {

        private readonly Action<Func<string>> onMessage;
        internal XRunCallback(Action<Func<string>> onMessage) {
            this.onMessage = onMessage;
        }

        internal void OnCallback(XtStream stream, bool output, bool overflow, int frames, object user) {
            onMessage(() => string.Format(
                "{0} {1} of size {2} on device {3}, user: {4}.",
                output ? "Output" : "Input",
                overflow ? "overflow" : "underflow",
                frames,
                stream.GetDevice() == null ? "null" : stream.GetDevice().GetName(),
                user));
        }
    }
}