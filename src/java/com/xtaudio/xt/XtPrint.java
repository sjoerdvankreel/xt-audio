package com.xtaudio.xt;

/* Copyright (C) 2015-2017 Sjoerd van Kreel.
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
public final class XtPrint {

    private XtPrint() {
    }

    public static String errorToString(long error) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintErrorToString(error));
    }

    public static String mixToString(XtMix mix) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintMixToString(XtNative.Mix.toNative(mix)));
    }

    public static String levelToString(XtLevel level) {
        return XtNative.XtPrintLevelToString(level.ordinal());
    }

    public static String causeToString(XtCause cause) {
        return XtNative.XtPrintCauseToString(cause.ordinal());
    }

    public static String setupToString(XtSetup setup) {
        return XtNative.XtPrintSetupToString(setup.ordinal());
    }

    public static String systemToString(XtSystem system) {
        return XtNative.XtPrintSystemToString(system.ordinal() + 1);
    }

    public static String sampleToString(XtSample sample) {
        return XtNative.XtPrintSampleToString(sample.ordinal());
    }

    public static String formatToString(XtFormat format) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintFormatToString(XtNative.Format.toNative(format)));
    }

    public static String bufferToString(XtBuffer buffer) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintBufferToString(buffer));
    }

    public static String latencyToString(XtLatency latency) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintLatencyToString(latency));
    }
    
    public static String channelsToString(XtChannels channels) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintChannelsToString(channels));
    }

    public static String capabilitiesToString(int capabilities) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintCapabilitiesToString(capabilities));
    }

    public static String attributesToString(XtAttributes attributes) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintAttributesToString(attributes));
    }
}
