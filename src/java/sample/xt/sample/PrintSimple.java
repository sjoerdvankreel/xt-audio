package xt.sample;

import java.util.EnumSet;
import xt.audio.Enums.XtEnumFlags;
import xt.audio.Enums.XtSystem;
import xt.audio.XtAudio;
import xt.audio.XtDeviceList;
import xt.audio.XtPlatform;
import xt.audio.XtService;

public class PrintSimple {

    public static void main() throws Exception {
        try(XtPlatform platform = XtAudio.init(null, null)) {
            for(XtSystem system: platform.getSystems()) {
                XtService service = platform.getService(system);
                try(XtDeviceList list = service.openDeviceList(EnumSet.of(XtEnumFlags.ALL))) {
                    for(int d = 0; d < list.getCount(); d++) {
                        String id = list.getId(d);
                        System.out.println(system + ": " + list.getName(id));
                    }
                }
            }
        }
    }
}