package com.xtaudio.xt;

import com.sun.jna.Structure;
import java.util.Arrays;
import java.util.List;

public class XtAttributes extends Structure {

    public static class ByValue extends XtAttributes implements Structure.ByValue {}
    
    public int size;
    public boolean isFloat;
    public boolean isSigned;

    @Override
    public String toString() {
        return XtPrint.attributesToString(this);
    }

    @Override
    protected List getFieldOrder() {
        return Arrays.asList("size", "isFloat", "isSigned");
    }
}