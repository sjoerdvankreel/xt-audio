package com.xtaudio.xt.sample;

import com.xtaudio.xt.XtAudio;
import com.xtaudio.xt.XtDevice;
import com.xtaudio.xt.XtService;

public class PrintSimple {

    public static void main(String[] args) {

        try (XtAudio audio = new XtAudio(null, null, null, null)) {
            for (int s = 0; s < XtAudio.getServiceCount(); s++) {
                XtService service = XtAudio.getServiceByIndex(s);
                for (int d = 0; d < service.getDeviceCount(); d++)
                    try (XtDevice device = service.openDevice(d)) {
                        System.out.println(service.getName() + ": " + device.getName());
                    }
            }
        }
    }
}
