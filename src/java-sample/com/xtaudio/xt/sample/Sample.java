package com.xtaudio.xt.sample;

import com.xtaudio.xt.XtException;
import com.xtaudio.xt.XtPrint;

public class Sample {

    public static void main(String[] args) throws Exception {
        try {
            System.out.println("PrintSimple:");
            PrintSimple.main(args);
            System.out.println("PrintDetailed:");
            PrintDetailed.main(args);
            System.out.println("CaptureSimple:");
            CaptureSimple.main(args);
            System.out.println("RenderSimple:");
            RenderSimple.main(args);
            System.out.println("CaptureAdvanced:");
            CaptureAdvanced.main(args);
            System.out.println("RenderAdvanced:");
            RenderAdvanced.main(args);
            System.out.println("FullDuplex:");
            FullDuplex.main(args);
            System.out.println("Aggregate:");
            Aggregate.main(args);
        } catch (XtException e) {
            System.out.println("Error:");
            System.out.println("\tSystem: " + XtPrint.systemToString(XtException.getSystem(e.getError())));
            System.out.println("\tCause: " + XtPrint.causeToString(XtException.getCause(e.getError())));
            System.out.println("\tFault: " + XtException.getFault(e.getError()));
            System.out.println("\tText: " + XtException.getText(e.getError()));
        }
    }
}
