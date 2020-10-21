package com.xtaudio.xt;

public final class XtException extends RuntimeException {

    private final long error;

    XtException(long error) {
        this.error = error;
    }

    public long getError() {
        return error;
    }

    @Override
    public String toString() {
        return XtPrint.errorToString(getError());
    }

    public static XtCause getCause(long error) {
        return XtCause.class.getEnumConstants()[XtNative.XtAudioGetErrorCause(error)];
    }

    public static XtSystem getSystem(long error) {
        return XtSystem.class.getEnumConstants()[XtNative.XtAudioGetErrorSystem(error) - 1];
    }

    public static int getFault(long error) {
        return XtNative.XtAudioGetErrorFault(error);
    }

    public static String getText(long error) {
        return XtNative.XtAudioGetErrorText(error);
    }
}