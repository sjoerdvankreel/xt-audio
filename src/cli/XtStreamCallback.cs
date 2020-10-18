namespace Xt
{
    public delegate void XtStreamCallback(XtStream stream,
        object input, object output, int frames, double time,
        ulong position, bool timeValid, ulong error, object user);
}