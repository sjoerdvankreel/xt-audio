using System;

namespace Xt
{
    public class OnBufferParams
    {
        public bool Native { get; }
        public bool Interleaved { get; }
        public string Name { get; set; }
        public Action<Func<string>> OnMessage { get; }

        public OnBufferParams(bool interleaved, bool native, Action<Func<string>> onMessage)
        => (Interleaved, Native, OnMessage) = (interleaved, native, onMessage);
    }
}