using System;

namespace Xt
{
    public class OnXRun
    {
        readonly Action<Func<string>> _onMessage;
        public OnXRun(Action<Func<string>> onMessage)
        => _onMessage = onMessage;

        public void Callback(XtStream stream, int index, object user)
        => _onMessage(() => $"XRun on stream {index}.");
    }
}