package com.xtaudio.xt;

public final class XtException extends RuntimeException {

    private final long error;

    XtException(long error) {
        this.error = error;
    }

    public long getError() {
        return error;
    }
}