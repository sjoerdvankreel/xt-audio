package com.xtaudio.xt;

public enum XtCause {

    FORMAT,
    SERVICE,
    GENERIC,
    UNKNOWN,
    ENDPOINT;

    @Override
    public String toString() {
        return XtPrint.causeToString(this);
    }
}