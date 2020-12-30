using System;

namespace Xt
{
    public unsafe class OnRender : OnBuffer
    {
        static readonly double Frequency = 440.0;

        double _phase;
        public OnRender(OnBufferParams @params) : base(@params) { }

        protected override void ProcessBuffer(XtStream stream, in XtBuffer buffer, XtSafeBuffer safe)
        {
            var frames = buffer.frames;
            var format = stream.GetFormat();
            for (int f = 0; f < frames; f++)
            {
                int ival;
                double val = Math.Sin(_phase * 2.0 * Math.PI);
                _phase += Frequency / format.mix.rate;
                if (_phase >= 1.0) _phase = -1.0;
                if (Params.Native && Params.Interleaved) ProcessInterleavedNative(in format, f, val, buffer.output);
                if (Params.Native && !Params.Interleaved) ProcessNonInterleavedNative(in format, f, val, buffer.output);
                if (!Params.Native && Params.Interleaved) ProcessInterleavedSafe(in format, f, val, safe.GetOutput());
                if (!Params.Native && !Params.Interleaved) ProcessNonInterleavedSafe(in format, f, val, safe.GetOutput());
            }
        }

        void ProcessInterleavedSafe(in XtFormat format, int frame, double val, Array output)
        {
            int ival;
            for (int c = 0; c < format.channels.outputs; c++)
            {
                int pos = frame * format.channels.outputs + c;
                switch (format.mix.sample)
                {
                case XtSample.Float32: ((float[])output)[pos] = (float)val; break;
                case XtSample.Int32: ((int[])output)[pos] = (int)(val * int.MaxValue); break;
                case XtSample.Int16: ((short[])output)[pos] = (short)(val * short.MaxValue); break;
                case XtSample.UInt8: ((byte[])output)[pos] = (byte)((val * 0.5 + 0.5) * byte.MaxValue); break;
                case XtSample.Int24:
                ival = (int)(val * int.MaxValue);
                ((byte[])output)[pos * 3] = (byte)((ival & 0x0000FF00) >> 8);
                ((byte[])output)[pos * 3 + 1] = (byte)((ival & 0x00FF0000) >> 16);
                ((byte[])output)[pos * 3 + 2] = (byte)((ival & 0xFF000000) >> 24);
                break;
                default: throw new InvalidOperationException();
                }
            }
        }

        void ProcessInterleavedNative(in XtFormat format, int frame, double val, IntPtr output)
        {
            int ival;
            for (int c = 0; c < format.channels.outputs; c++)
            {
                int pos = frame * format.channels.outputs + c;
                switch (format.mix.sample)
                {
                case XtSample.Float32: ((float*)output)[pos] = (float)val; break;
                case XtSample.Int32: ((int*)output)[pos] = (int)(val * int.MaxValue); break;
                case XtSample.Int16: ((short*)output)[pos] = (short)(val * short.MaxValue); break;
                case XtSample.UInt8: ((byte*)output)[pos] = (byte)((val * 0.5 + 0.5) * byte.MaxValue); break;
                case XtSample.Int24:
                ival = (int)(val * int.MaxValue);
                ((byte*)output)[pos * 3] = (byte)((ival & 0x0000FF00) >> 8);
                ((byte*)output)[pos * 3 + 1] = (byte)((ival & 0x00FF0000) >> 16);
                ((byte*)output)[pos * 3 + 2] = (byte)((ival & 0xFF000000) >> 24);
                break;
                default: throw new InvalidOperationException();
                }
            }
        }

        void ProcessNonInterleavedSafe(in XtFormat format, int frame, double val, Array output)
        {
            int ival;
            for (int c = 0; c < format.channels.outputs; c++)
            {
                switch (format.mix.sample)
                {
                case XtSample.Float32: ((float[][])output)[c][frame] = (float)val; break;
                case XtSample.Int32: ((int[][])output)[c][frame] = (int)(val * int.MaxValue); break;
                case XtSample.Int16: ((short[][])output)[c][frame] = (short)(val * short.MaxValue); break;
                case XtSample.UInt8: ((byte[][])output)[c][frame] = (byte)((val * 0.5 + 0.5) * byte.MaxValue); break;
                case XtSample.Int24:
                ival = (int)(val * int.MaxValue);
                ((byte[][])output)[c][frame * 3] = (byte)((ival & 0x0000FF00) >> 8);
                ((byte[][])output)[c][frame * 3 + 1] = (byte)((ival & 0x00FF0000) >> 16);
                ((byte[][])output)[c][frame * 3 + 2] = (byte)((ival & 0xFF000000) >> 24);
                break;
                default: throw new InvalidOperationException();
                }
            }
        }

        void ProcessNonInterleavedNative(in XtFormat format, int frame, double val, IntPtr output)
        {
            int ival;
            for (int c = 0; c < format.channels.outputs; c++)
            {
                switch (format.mix.sample)
                {
                case XtSample.Float32: ((float**)output)[c][frame] = (float)val; break;
                case XtSample.Int32: ((int**)output)[c][frame] = (int)(val * int.MaxValue); break;
                case XtSample.Int16: ((short**)output)[c][frame] = (short)(val * short.MaxValue); break;
                case XtSample.UInt8: ((byte**)output)[c][frame] = (byte)((val * 0.5 + 0.5) * byte.MaxValue); break;
                case XtSample.Int24:
                ival = (int)(val * int.MaxValue);
                ((byte**)output)[c][frame * 3] = (byte)((ival & 0x0000FF00) >> 8);
                ((byte**)output)[c][frame * 3 + 1] = (byte)((ival & 0x00FF0000) >> 16);
                ((byte**)output)[c][frame * 3 + 2] = (byte)((ival & 0xFF000000) >> 24);
                break;
                default: throw new InvalidOperationException();
                }
            }
        }
    }
}