using System;

namespace Xt
{
    class XRunCallback
    {
        private readonly Action<Func<string>> onMessage;
        internal XRunCallback(Action<Func<string>> onMessage)
        {
            this.onMessage = onMessage;
        }

        internal void OnCallback(int index, IntPtr user)
        {
            onMessage(() => string.Format("XRun on stream {0}", index));
        }
    }
}