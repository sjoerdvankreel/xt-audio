package com.xtaudio.xt;

import com.sun.jna.Structure;
import java.util.Arrays;
import java.util.List;

public final class XtLatency extends Structure {

    public double input;
    public double output;

    @Override
    protected List getFieldOrder() {
        return Arrays.asList("input", "output");
    }
}