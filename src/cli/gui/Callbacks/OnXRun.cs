using System;

namespace Xt
{
    class OnXRun
    {
        readonly Action<Func<string>> _onMessage;
        internal OnXRun(Action<Func<string>> onMessage) 
        => _onMessage = onMessage;

        internal void Callback(XtStream stream, int index, object user)
        => _onMessage(() => $"XRun on stream {index}.");
    }
}