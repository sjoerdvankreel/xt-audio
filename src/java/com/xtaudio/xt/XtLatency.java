package com.xtaudio.xt;

import com.sun.jna.Structure;
import java.util.Arrays;
import java.util.List;

/* Copyright (C) 2015-2020 Sjoerd van Kreel.
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
public final class XtLatency extends Structure {

    public double input;
    public double output;
    
    @Override
    public String toString() {
        return XtPrint.latencyToString(this);
    }
    
    @Override
    protected List getFieldOrder() {
        return Arrays.asList("input", "output");
    }
}
