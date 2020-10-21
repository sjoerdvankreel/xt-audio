package com.xtaudio.xt;

import com.sun.jna.Structure;
import com.sun.jna.TypeMapper;
import java.util.Arrays;
import java.util.List;

public final class XtMix extends Structure {    
    
    public final static TypeMapper TYPE_MAPPER = new XtNative.TypeMapper();
    
    public int rate;
    public XtSample sample;

    public XtMix() {
    }

    public XtMix(int rate, XtSample sample) {
        this.rate = rate;
        this.sample = sample;
    }
    
    @Override
    protected List getFieldOrder() {
        return Arrays.asList("rate", "sample");
    }
}