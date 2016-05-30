package com.xtaudio.xt;

public class Sample {

    public static void main(String[] args) throws Exception {

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
    }
}
