package xt.sample;

import xt.audio.XtAudio;
import xt.audio.XtException;

public final class Sample {

    interface Runner {
        void run() throws Exception;
    }

    static void runSample(int index) throws Exception {
        System.out.println(NAMES[index] + ":");
        SAMPLES[index].run();
    }

    static final String[] NAMES = new String[]{
            "PrintSimple", "PrintDetailed", "CaptureSimple", "RenderSimple",
            "CaptureAdvanced", "RenderAdvanced", "FullDuplex", "Aggregate"
    };

    static final Runner[] SAMPLES = new Runner[]{
            PrintSimple::main, PrintDetailed::main, CaptureSimple::main, RenderSimple::main,
            CaptureAdvanced::main, RenderAdvanced::main, FullDuplex::main, Aggregate::main
    };

    public static void main(String[] args) throws Exception {
        int index = args.length == 1? Integer.parseInt(args[0]): -1;
        try {
            if(index >= 0) runSample(index);
            else for(int i = 0; i < SAMPLES.length; i++) runSample(i);
        } catch(XtException e) {
            System.out.println(XtAudio.getErrorInfo(e.getError()));
        } catch(Throwable t) {
            System.out.println(t.getMessage());
        }
    }
}