package com.xtaudio.xt;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import java.util.Arrays;
import java.util.List;

public class XtChannels extends Structure {

    public int inputs;
    public long inMask;
    public int outputs;
    public long outMask;

    public XtChannels() {
    }

    public XtChannels(int inputs, long inMask, int outputs, long outMask) {
        this.inputs = inputs;
        this.inMask = inMask;
        this.outputs = outputs;
        this.outMask = outMask;
    }

    void doUseMemory(Pointer m, int offset) {
        super.useMemory(m, offset);
    }

    @Override
    public String toString() {
        return XtPrint.channelsToString(this);
    }

    @Override
    protected List getFieldOrder() {
        return Arrays.asList("inputs", "inMask", "outputs", "outMask");
    }
}