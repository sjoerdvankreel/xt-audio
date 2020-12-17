using System;

namespace Xt
{
    class OnXRun
    {
        private readonly Action<Func<string>> onMessage;
        internal OnXRun(Action<Func<string>> onMessage)
        {
            this.onMessage = onMessage;
        }

        internal void OnCallback(XtStream stream, int index, object user)
        {
            onMessage(() => string.Format("XRun on stream {0}", index));
        }
    }
}