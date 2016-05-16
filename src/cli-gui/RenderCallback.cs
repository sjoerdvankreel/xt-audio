using System;

namespace Xt {

    class RenderCallback : StreamCallback {

        private double phase;
        private static readonly double Frequency = 660.0;

        internal RenderCallback(Action<string> onError, Action<string> onMessage) :
            base("Render", onError, onMessage) {
        }

        internal override void OnCallback(XtFormat format, bool interleaved, Array input, Array output, int frames) {

            for (int f = 0; f < frames; f++) {

                int ival;
                double val = Math.Sin(phase * Math.PI);
                phase += Frequency / format.mix.rate;
                if (phase >= 1.0)
                    phase = -1.0;

                if (interleaved)
                    for (int c = 0; c < format.outputs; c++) {
                        int pos = f * format.outputs + c;
                        switch (format.mix.sample) {
                            case XtSample.UInt8:
                                ((byte[])output)[pos] = (byte)((val * 0.5 + 0.5) * byte.MaxValue);
                                break;
                            case XtSample.Int16:
                                ((short[])output)[pos] = (short)(val * short.MaxValue);
                                break;
                            case XtSample.Int24:
                                ival = (int)(val * int.MaxValue);
                                ((byte[])output)[pos * 3] = (byte)((ival & 0x0000FF00) >> 8);
                                ((byte[])output)[pos * 3 + 1] = (byte)((ival & 0x00FF0000) >> 16);
                                ((byte[])output)[pos * 3 + 2] = (byte)((ival & 0xFF000000) >> 24);
                                break;
                            case XtSample.Int32:
                                ((int[])output)[pos] = (int)(val * int.MaxValue);
                                break;
                            case XtSample.Float32:
                                ((float[])output)[pos] = (float)val;
                                break;
                            default:
                                break;
                        }
                    }
                else
                    for (int c = 0; c < format.outputs; c++) {
                        switch (format.mix.sample) {
                            case XtSample.UInt8:
                                ((byte[][])output)[c][f] = (byte)((val * 0.5 + 0.5) * byte.MaxValue);
                                break;
                            case XtSample.Int16:
                                ((short[][])output)[c][f] = (short)(val * short.MaxValue);
                                break;
                            case XtSample.Int24:
                                ival = (int)(val * int.MaxValue);
                                ((byte[][])output)[c][f * 3] = (byte)((ival & 0x0000FF00) >> 8);
                                ((byte[][])output)[c][f * 3 + 1] = (byte)((ival & 0x00FF0000) >> 16);
                                ((byte[][])output)[c][f * 3 + 2] = (byte)((ival & 0xFF000000) >> 24);
                                break;
                            case XtSample.Int32:
                                ((int[][])output)[c][f] = (int)(val * int.MaxValue);
                                break;
                            case XtSample.Float32:
                                ((float[][])output)[c][f] = (float)val;
                                break;
                            default:
                                break;
                        }
                    }
            }
        }
    }
}
