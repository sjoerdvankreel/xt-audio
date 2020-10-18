package com.xtaudio.xt;

public enum XtSystem {

    ALSA,
    ASIO,
    JACK,
    PULSE,
    DSOUND,
    WASAPI;

    @Override
    public String toString() {
        return XtPrint.systemToString(this);
    }
}