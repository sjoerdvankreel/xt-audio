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

public final class XtFormat {

    public XtMix mix = new XtMix();
    public XtChannels channels = new XtChannels();

    public XtFormat() {
    }

    public XtFormat(XtMix mix, XtChannels channels) {
        this.mix = mix;
        this.channels = channels;
    }

    @Override
    public String toString() {
        return XtPrint.formatToString(this);
    }
}