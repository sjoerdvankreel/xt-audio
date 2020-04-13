package com.xtaudio.xt;

import com.sun.jna.Pointer;

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
public final class XtAudio implements XtCloseable {

    static XtTraceCallback trace;
    private static XtFatalCallback fatal;
    private static XtNative.TraceCallback nativeTrace;

    private static void ForwardTrace(int level, String message) {
        if (trace != null)
            trace.callback(XtLevel.class.getEnumConstants()[level], message);
    }

    public XtAudio(String id, Pointer window, XtTraceCallback trace, XtFatalCallback fatal) {
        XtAudio.trace = trace;
        XtAudio.fatal = fatal;
        XtAudio.nativeTrace = XtAudio::ForwardTrace;
        XtNative.init();
        XtNative.XtAudioInit(id, window, nativeTrace, fatal);
    }

    @Override
    public void close() {
        XtNative.XtAudioTerminate();
    }

    public static boolean isWin32() {
        return XtNative.XtAudioIsWin32();
    }

    public static String getVersion() {
        return XtNative.XtAudioGetVersion();
    }

    public static int getServiceCount() {
        return XtNative.XtAudioGetServiceCount();
    }

    public static XtService getServiceByIndex(int index) {
        return new XtService(XtNative.XtAudioGetServiceByIndex(index));
    }

    public static XtService getServiceBySetup(XtSetup setup) {
        Pointer service = XtNative.XtAudioGetServiceBySetup(setup.ordinal());
        return service == Pointer.NULL ? null : new XtService(service);
    }

    public static XtService getServiceBySystem(XtSystem system) {
        Pointer service = XtNative.XtAudioGetServiceBySystem(system.ordinal() + 1);
        return service == Pointer.NULL ? null : new XtService(service);
    }

    public static XtAttributes getSampleAttributes(XtSample sample) {
        XtAttributes result = new XtAttributes();
        XtNative.XtAudioGetSampleAttributes(sample.ordinal(), result);
        return result;
    }
}
