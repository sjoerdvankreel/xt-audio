package xt.audio;

import com.sun.jna.Callback;
import com.sun.jna.Pointer;

interface NativeCallbacks {

    interface OnXRun extends Callback {
        void callback(Pointer stream, int index, Pointer user) throws Exception;
    }

    interface OnBuffer extends Callback {
        int callback(Pointer stream, Pointer buffer, Pointer user) throws Exception;
    }

    interface OnRunning extends Callback {
        void callback(Pointer stream, boolean running, long error, Pointer user) throws Exception;
    }
}