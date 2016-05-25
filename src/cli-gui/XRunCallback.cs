using System;

namespace Xt {

    class XRunCallback {

        private readonly Action<Func<string>> onMessage;
        internal XRunCallback(Action<Func<string>> onMessage) {
            this.onMessage = onMessage;
        }

        internal void OnCallback(int index, bool output, bool overflow, int frames, object user) {
            onMessage(() => string.Format(
                "{0} {1} of size {2} on stream {3}, user: {4}.",
                output ? "Output" : "Input",
                overflow ? "overflow" : "underflow",
                frames, index, user));
        }
    }
}