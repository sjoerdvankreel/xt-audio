package com.xtaudio.xt;

public enum XtSample {

    UINT8,
    INT16,
    INT24,
    INT32,
    FLOAT32;

    @Override
    public String toString() {
        return XtPrint.sampleToString(this);
    }
}