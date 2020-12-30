using System;

namespace Xt
{
    public unsafe class OnAggregate : OnBuffer
    {
        double _attenuate = 1.0;
        double[] _aggregateChannel;

        public OnAggregate(OnBufferParams @params) : base(@params) { }
        public void Init(int maxFrames) => _aggregateChannel = new double[maxFrames];

        protected override void ProcessBuffer(XtStream stream, in XtBuffer buffer, XtSafeBuffer safe)
        {
            /*
            bool native = Params.Native;
            var format = stream.GetFormat();
            bool interleaved = Params.Interleaved;
            Array.Clear(_aggregateChannel, 0, buffer.frames);
            for (int f = 0; f < buffer.frames; f++)
            {
                for (int c = 0; c < format.channels.inputs; c++)
                    if (!native && !interleaved)
                        switch (format.mix.sample)
                        {
                        case XtSample.UInt8:
                        _aggregateChannel[f] += (((((byte[][])input)[c][f]) * 2.0) - 1.0) / byte.MaxValue;
                        break;
                        case XtSample.Int16:
                        _aggregateChannel[f] += (((short[][])input)[c][f]) / (double)short.MaxValue;
                        break;
                        case XtSample.Int24:
                        int value = (int)(
                        ((byte[][])input)[c][f * 3] << 8 |
                        ((byte[][])input)[c][f * 3 + 1] << 16 |
                        ((byte[][])input)[c][f * 3 + 2] << 24);
                        _aggregateChannel[f] += value / (double)int.MaxValue;
                        break;
                        case XtSample.Int32:
                        _aggregateChannel[f] += (((int[][])input)[c][f]) / (double)int.MaxValue;
                        break;
                        case XtSample.Float32:
                        _aggregateChannel[f] += ((float[][])input)[c][f];
                        break;
                        } else if (!native && interleaved)
                        switch (format.mix.sample)
                        {
                        case XtSample.UInt8:
                        _aggregateChannel[f] += (((((byte[])input)[f * format.channels.inputs + c]) * 2.0) - 1.0) / byte.MaxValue;
                        break;
                        case XtSample.Int16:
                        _aggregateChannel[f] += (((short[])input)[f * format.channels.inputs + c]) / (double)short.MaxValue;
                        break;
                        case XtSample.Int24:
                        int value = (int)(
                        ((byte[])input)[(f * format.channels.inputs + c) * 3] << 8 |
                        ((byte[])input)[(f * format.channels.inputs + c) * 3 + 1] << 16 |
                        ((byte[])input)[(f * format.channels.inputs + c) * 3 + 2] << 24);
                        _aggregateChannel[f] += value / (double)int.MaxValue;
                        break;
                        case XtSample.Int32:
                        _aggregateChannel[f] += (((int[])input)[f * format.channels.inputs + c]) / (double)int.MaxValue;
                        break;
                        case XtSample.Float32:
                        _aggregateChannel[f] += ((float[])input)[f * format.channels.inputs + c];
                        break;
                        } else if (native && !interleaved)
                        switch (format.mix.sample)
                        {
                        case XtSample.UInt8:
                        _aggregateChannel[f] += (((((byte**)(IntPtr)input)[c][f]) * 2.0) - 1.0) / byte.MaxValue;
                        break;
                        case XtSample.Int16:
                        _aggregateChannel[f] += (((short**)(IntPtr)input)[c][f]) / (double)short.MaxValue;
                        break;
                        case XtSample.Int24:
                        int value = (int)(
                        ((byte**)(IntPtr)input)[c][f * 3] << 8 |
                        ((byte**)(IntPtr)input)[c][f * 3 + 1] << 16 |
                        ((byte**)(IntPtr)input)[c][f * 3 + 2] << 24);
                        _aggregateChannel[f] += value / (double)int.MaxValue;
                        break;
                        case XtSample.Int32:
                        _aggregateChannel[f] += (((int**)(IntPtr)input)[c][f]) / (double)int.MaxValue;
                        break;
                        case XtSample.Float32:
                        _aggregateChannel[f] += ((float**)(IntPtr)input)[c][f];
                        break;
                        } else
                        switch (format.mix.sample)
                        {
                        case XtSample.UInt8:
                        _aggregateChannel[f] += (((((byte*)(IntPtr)input)[f * format.channels.inputs + c]) * 2.0) - 1.0) / byte.MaxValue;
                        break;
                        case XtSample.Int16:
                        _aggregateChannel[f] += (((short*)(IntPtr)input)[f * format.channels.inputs + c]) / (double)short.MaxValue;
                        break;
                        case XtSample.Int24:
                        int value = (int)(
                        ((byte*)(IntPtr)input)[(f * format.channels.inputs + c) * 3] << 8 |
                        ((byte*)(IntPtr)input)[(f * format.channels.inputs + c) * 3 + 1] << 16 |
                        ((byte*)(IntPtr)input)[(f * format.channels.inputs + c) * 3 + 2] << 24);
                        _aggregateChannel[f] += value / (double)int.MaxValue;
                        break;
                        case XtSample.Int32:
                        _aggregateChannel[f] += (((int*)(IntPtr)input)[f * format.channels.inputs + c]) / (double)int.MaxValue;
                        break;
                        case XtSample.Float32:
                        _aggregateChannel[f] += ((float*)(IntPtr)input)[f * format.channels.inputs + c];
                        break;
                        }
                _attenuate = Math.Min(_attenuate, 1.0 / Math.Abs(_aggregateChannel[f]));
                _aggregateChannel[f] *= _attenuate;
                for (int c = 0; c < format.channels.outputs; c++)
                    if (!native && !interleaved)
                        switch (format.mix.sample)
                        {
                        case XtSample.UInt8:
                        ((byte[][])output)[c][f] = (byte)(((_aggregateChannel[f] + 1.0) * 0.5) * byte.MaxValue);
                        break;
                        case XtSample.Int16:
                        ((short[][])output)[c][f] = (short)(_aggregateChannel[f] * (double)short.MaxValue);
                        break;
                        case XtSample.Int24:
                        int value = (int)(_aggregateChannel[f] * (double)int.MaxValue);
                        ((byte[][])output)[c][f * 3] = (byte)((value & 0x0000FF00) >> 8);
                        ((byte[][])output)[c][f * 3 + 1] = (byte)((value & 0x00FF0000) >> 16);
                        ((byte[][])output)[c][f * 3 + 2] = (byte)((value & 0xFF000000) >> 24);
                        break;
                        case XtSample.Int32:
                        ((int[][])output)[c][f] = (int)(_aggregateChannel[f] * (double)int.MaxValue);
                        break;
                        case XtSample.Float32:
                        ((float[][])output)[c][f] = (float)_aggregateChannel[f];
                        break;
                        } else if (!native && interleaved)
                        switch (format.mix.sample)
                        {
                        case XtSample.UInt8:
                        ((byte[])output)[f * format.channels.outputs + c] = (byte)(((_aggregateChannel[f] + 1.0) * 0.5) * byte.MaxValue);
                        break;
                        case XtSample.Int16:
                        ((short[])output)[f * format.channels.outputs + c] = (short)(_aggregateChannel[f] * (double)short.MaxValue);
                        break;
                        case XtSample.Int24:
                        int value = (int)(_aggregateChannel[f] * (double)int.MaxValue);
                        ((byte[])output)[(f * format.channels.outputs + c) * 3] = (byte)((value & 0x0000FF00) >> 8);
                        ((byte[])output)[(f * format.channels.outputs + c) * 3 + 1] = (byte)((value & 0x00FF0000) >> 16);
                        ((byte[])output)[(f * format.channels.outputs + c) * 3 + 2] = (byte)((value & 0xFF000000) >> 24);
                        break;
                        case XtSample.Int32:
                        ((int[])output)[f * format.channels.outputs + c] = (int)(_aggregateChannel[f] * (double)int.MaxValue);
                        break;
                        case XtSample.Float32:
                        ((float[])output)[f * format.channels.outputs + c] = (float)_aggregateChannel[f];
                        break;
                        } else if (native && !interleaved)
                        switch (format.mix.sample)
                        {
                        case XtSample.UInt8:
                        ((byte**)(IntPtr)output)[c][f] = (byte)(((_aggregateChannel[f] + 1.0) * 0.5) * byte.MaxValue);
                        break;
                        case XtSample.Int16:
                        ((short**)(IntPtr)output)[c][f] = (short)(_aggregateChannel[f] * (double)short.MaxValue);
                        break;
                        case XtSample.Int24:
                        int value = (int)(_aggregateChannel[f] * (double)int.MaxValue);
                        ((byte**)(IntPtr)output)[c][f * 3] = (byte)((value & 0x0000FF00) >> 8);
                        ((byte**)(IntPtr)output)[c][f * 3 + 1] = (byte)((value & 0x00FF0000) >> 16);
                        ((byte**)(IntPtr)output)[c][f * 3 + 2] = (byte)((value & 0xFF000000) >> 24);
                        break;
                        case XtSample.Int32:
                        ((int**)(IntPtr)output)[c][f] = (int)(_aggregateChannel[f] * (double)int.MaxValue);
                        break;
                        case XtSample.Float32:
                        ((float**)(IntPtr)output)[c][f] = (float)_aggregateChannel[f];
                        break;
                        } else
                        switch (format.mix.sample)
                        {
                        case XtSample.UInt8:
                        ((byte*)(IntPtr)output)[f * format.channels.outputs + c] = (byte)(((_aggregateChannel[f] + 1.0) * 0.5) * byte.MaxValue);
                        break;
                        case XtSample.Int16:
                        ((short*)(IntPtr)output)[f * format.channels.outputs + c] = (short)(_aggregateChannel[f] * (double)short.MaxValue);
                        break;
                        case XtSample.Int24:
                        int value = (int)(_aggregateChannel[f] * int.MaxValue);
                        ((byte*)(IntPtr)output)[(f * format.channels.outputs + c) * 3] = (byte)((value & 0x0000FF00) >> 8);
                        ((byte*)(IntPtr)output)[(f * format.channels.outputs + c) * 3 + 1] = (byte)((value & 0x00FF0000) >> 16);
                        ((byte*)(IntPtr)output)[(f * format.channels.outputs + c) * 3 + 2] = (byte)((value & 0xFF000000) >> 24);
                        break;
                        case XtSample.Int32:
                        ((int*)(IntPtr)output)[f * format.channels.outputs + c] = (int)(_aggregateChannel[f] * int.MaxValue);
                        break;
                        case XtSample.Float32:
                        ((float*)(IntPtr)output)[f * format.channels.outputs + c] = (float)_aggregateChannel[f];
                        break;
                        }
            }
            */
        }
    }
}