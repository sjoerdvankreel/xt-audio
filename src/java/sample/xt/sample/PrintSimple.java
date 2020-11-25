package xt.sample;

import xt.audio.CoreEnums.XtSystem;
import xt.audio.XtAudio;
import xt.audio.XtDevice;
import xt.audio.XtPlatform;
import xt.audio.XtService;

public class PrintSimple {

    public static void main() throws Exception {
        try(XtPlatform platform = XtAudio.init(null, null, null)) {
            for(XtSystem system: platform.getSystems()) {
                XtService service = platform.getService(system);
                for(int d = 0; d < service.getDeviceCount(); d++)
                    try(XtDevice device = service.openDevice(d)) {
                        System.out.println(system + ": " + device);
                    }
            }
        }
    }
}