package com.xtaudio.xt;

import com.sun.jna.Pointer;

public class PrintDeviceNames {

    public static void main(String[] args) {
        
        try (XtAudio audio = new XtAudio(null, Pointer.NULL, null, null)) {
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
