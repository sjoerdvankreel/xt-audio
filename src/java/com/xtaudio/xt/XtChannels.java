package com.xtaudio.xt;

import com.sun.jna.Structure;
import java.util.Arrays;
import java.util.List;

/* Copyright (C) 2015-2016 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */
public final class XtChannels extends Structure {
    
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

    @Override
    public String toString() {
        return XtPrint.channelsToString(this);
    }
    
    @Override
    protected List getFieldOrder() {
        return Arrays.asList("inputs", "inMask", "outputs", "outMask");
    }
}
