using System;

namespace Xt {

    class XRunCallback {

        private readonly Action<Func<string>> onMessage;
        internal XRunCallback(Action<Func<string>> onMessage) {
            this.onMessage = onMessage;
        }

        internal void OnCallback(int index, object user) {
            onMessage(() => string.Format("XRun on stream {0}, user: {1}", index, user));
        }
    }
}