package com.xtaudio.xt;

public final class XtMix {

    public int rate;
    public XtSample sample;

    public XtMix() {
    }

    public XtMix(int rate, XtSample sample) {
        this.rate = rate;
        this.sample = sample;
    }

    @Override
    public String toString() {
        return XtPrint.mixToString(this);
    }
}