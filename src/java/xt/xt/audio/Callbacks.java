package xt.audio;

import xt.audio.Structs.XtBuffer;

public interface Callbacks {

    interface XtOnError  {
        void callback(String message) throws Exception;
    }

    interface XtOnXRun {
        void callback(XtStream stream, int index, Object user) throws Exception;
    }

    interface XtOnBuffer {
        int callback(XtStream stream, XtBuffer buffer, Object user) throws Exception;
    }

    interface XtOnRunning {
        void callback(XtStream stream, boolean running, long error, Object user) throws Exception;
    }
}