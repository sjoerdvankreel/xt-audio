package xt.audio;

import com.sun.jna.Callback;
import com.sun.jna.Pointer;
import com.sun.jna.win32.StdCall;


interface NativeCallbacks {

    interface NativeOnError extends Callback {
        void callback(String message) throws Exception;
    }

    interface NativeOnXRun extends Callback {
        void callback(Pointer stream, int index, Pointer user) throws Exception;
    }

    interface NativeOnBuffer extends Callback {
        int callback(Pointer stream, Pointer buffer, Pointer user) throws Exception;
    }

    interface NativeOnRunning extends Callback {
        void callback(Pointer stream, boolean running, long error, Pointer user) throws Exception;
    }
}