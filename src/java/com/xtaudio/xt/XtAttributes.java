package com.xtaudio.xt;

import com.sun.jna.Structure;
import java.util.Arrays;
import java.util.List;

public class XtAttributes extends Structure implements Structure.ByValue {
    
    public int size;
    public boolean isFloat;
    public boolean isSigned;

    @Override
    protected List getFieldOrder() {
        return Arrays.asList("size", "isFloat", "isSigned");
    }
}