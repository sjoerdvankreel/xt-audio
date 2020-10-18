package com.xtaudio.xt;

public enum XtSetup {

    PRO_AUDIO,
    SYSTEM_AUDIO,
    CONSUMER_AUDIO;

    @Override
    public String toString() {
        return XtPrint.setupToString(this);
    }
}