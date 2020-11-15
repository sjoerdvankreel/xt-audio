package com.xtaudio.xt.sample;

import com.xtaudio.xt.*;
import com.xtaudio.xt.NativeTypes.*;

public class PrintSimple {

    public static void main() throws Exception {
        try(AutoCloseable audio = XtAudio.init(null, null, null)) {
            for(XtSystem system: XtAudio.getSystems()) {
                XtService service = XtAudio.getService(system);
                for(int d = 0; d < service.getDeviceCount(); d++)
                    try(XtDevice device = service.openDevice(d)) {
                        System.out.println(system + ": " + device);
                    }
            }
        }
    }
}