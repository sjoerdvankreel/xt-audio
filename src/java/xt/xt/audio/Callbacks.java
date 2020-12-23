package xt.audio;

import com.sun.jna.Callback;
import com.sun.jna.Pointer;
import xt.audio.Structs.XtBuffer;
import xt.audio.Structs.XtLocation;

public interface Callbacks {

    interface OnXRun extends Callback {
        void callback(Pointer stream, int index, Pointer user) throws Exception;
    }

    interface OnBuffer extends Callback {
        int callback(Pointer stream, Pointer buffer, Pointer user) throws Exception;
    }

    interface OnRunning extends Callback {
        void callback(Pointer stream, boolean running, long error, Pointer user) throws Exception;
    }

    public interface XtOnXRun {
        void callback(XtStream stream, int index, Object user) throws Exception;
    }

    public interface XtOnError extends Callback {
        void callback(XtLocation location, String message) throws Exception;
    }

    public interface XtOnBuffer {
        int callback(XtStream stream, XtBuffer buffer, Object user) throws Exception;
    }

    public interface XtOnRunning {
        void callback(XtStream stream, boolean running, long error, Object user) throws Exception;
    }
}