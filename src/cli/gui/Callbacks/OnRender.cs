using System;

namespace Xt
{
    public class OnRender : OnBuffer
    {
        private double phase;
        private static readonly double Frequency = 440.0;
        public OnRender(OnBufferParams @params) : base(@params) { }

        protected override void ProcessBuffer(XtStream stream, in XtBuffer buffer, XtSafeBuffer safe)
        {/*
            for (int f = 0; f < frames; f++)
            {

                int ival;
                double val = Math.Sin(phase * 2.0 * Math.PI);
                phase += Frequency / format.mix.rate;
                if (phase >= 1.0)
                    phase = -1.0;

                if (!native)
                {
                    if (interleaved)
                        for (int c = 0; c < format.channels.outputs; c++)
                        {
                            int pos = f * format.channels.outputs + c;
                            switch (format.mix.sample)
                            {
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
                        for (int c = 0; c < format.channels.outputs; c++)
                        {
                            switch (format.mix.sample)
                            {
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
                } else
                {
                    if (interleaved)
                        for (int c = 0; c < format.channels.outputs; c++)
                        {
                            int pos = f * format.channels.outputs + c;
                            switch (format.mix.sample)
                            {
                            case XtSample.UInt8:
                            ((byte*)(IntPtr)output)[pos] = (byte)((val * 0.5 + 0.5) * byte.MaxValue);
                            break;
                            case XtSample.Int16:
                            ((short*)(IntPtr)output)[pos] = (short)(val * short.MaxValue);
                            break;
                            case XtSample.Int24:
                            ival = (int)(val * int.MaxValue);
                            ((byte*)(IntPtr)output)[pos * 3] = (byte)((ival & 0x0000FF00) >> 8);
                            ((byte*)(IntPtr)output)[pos * 3 + 1] = (byte)((ival & 0x00FF0000) >> 16);
                            ((byte*)(IntPtr)output)[pos * 3 + 2] = (byte)((ival & 0xFF000000) >> 24);
                            break;
                            case XtSample.Int32:
                            ((int*)(IntPtr)output)[pos] = (int)(val * int.MaxValue);
                            break;
                            case XtSample.Float32:
                            ((float*)(IntPtr)output)[pos] = (float)val;
                            break;
                            default:
                            break;
                            }
                        }
                    else
                        for (int c = 0; c < format.channels.outputs; c++)
                        {
                            switch (format.mix.sample)
                            {
                            case XtSample.UInt8:
                            ((byte**)(IntPtr)output)[c][f] = (byte)((val * 0.5 + 0.5) * byte.MaxValue);
                            break;
                            case XtSample.Int16:
                            ((short**)(IntPtr)output)[c][f] = (short)(val * short.MaxValue);
                            break;
                            case XtSample.Int24:
                            ival = (int)(val * int.MaxValue);
                            ((byte**)(IntPtr)output)[c][f * 3] = (byte)((ival & 0x0000FF00) >> 8);
                            ((byte**)(IntPtr)output)[c][f * 3 + 1] = (byte)((ival & 0x00FF0000) >> 16);
                            ((byte**)(IntPtr)output)[c][f * 3 + 2] = (byte)((ival & 0xFF000000) >> 24);
                            break;
                            case XtSample.Int32:
                            ((int**)(IntPtr)output)[c][f] = (int)(val * int.MaxValue);
                            break;
                            case XtSample.Float32:
                            ((float**)(IntPtr)output)[c][f] = (float)val;
                            break;
                            default:
                            break;
                            }
                        }
                }
            }
            */
        }
    }
}