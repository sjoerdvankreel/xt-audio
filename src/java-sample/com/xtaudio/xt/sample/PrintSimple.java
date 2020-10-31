package com.xtaudio.xt.sample;

import com.xtaudio.xt.XtAudio;

public class PrintSimple {

    public static void main(String[] args) {
        try(var audio = new XtAudio(null, null, null, null)) {
            for(var s : XtAudio.getSystems()) {
                var service = XtAudio.getService(s);
                for(int d = 0; d < service.getDeviceCount(); d++)
                    try(var device = service.openDevice(d)) {
                        System.out.println(service + ": " + device);
                    }
            }
        }
    }
}