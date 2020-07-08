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
package com.xtaudio.xt;

public final class XtCapabilities {

    private XtCapabilities() {
    }

    public static final int NONE = 0x0;
    public static final int TIME = 0x1;
    public static final int LATENCY = 0x2;
    public static final int FULL_DUPLEX = 0x4;
    public static final int CHANNEL_MASK = 0x8;
    public static final int XRUN_DETECTION = 0x10;
}
