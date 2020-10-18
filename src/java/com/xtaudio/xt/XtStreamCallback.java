package com.xtaudio.xt;

public interface XtStreamCallback {

    void callback(XtStream stream, Object input, Object output, int frames, double time,
            long position, boolean timeValid, long error, Object user) throws Exception;
}