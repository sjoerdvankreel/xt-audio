package com.xtaudio.xt;

import com.sun.jna.Structure;
import java.util.Arrays;
import java.util.List;

public final class XtBuffer extends Structure {

    public double min;
    public double max;
    public double current;

    @Override
    protected List getFieldOrder() {
        return Arrays.asList("min", "max", "current");
    }
}