package xt.audio;

import com.sun.jna.Callback;
import com.sun.jna.Pointer;
import xt.audio.Structs.XtBuffer;
import xt.audio.Structs.XtLocation;

public interface Callbacks {

    interface OnXRun extends Callback {
        void callback(int index, Pointer user) throws Exception;
    }

    public interface XtOnXRun {
        void callback(int index, Object user) throws Exception;
    }

    public interface XtOnError extends Callback {
        void callback(XtLocation location, String message) throws Exception;
    }

    interface OnBuffer extends Callback {
        void callback(Pointer stream, Pointer buffer, Pointer user) throws Exception;
    }

    public interface XtOnBuffer {
        void callback(XtStream stream, XtBuffer buffer, Object user) throws Exception;
    }
}