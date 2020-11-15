using System;
using System.Runtime.InteropServices;
using static Xt.XtNative;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    struct DeviceStreamParams
    {
        public StreamParams stream;
        public XtFormat format;
        public double bufferSize;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct AggregateDeviceParams
    {
        public IntPtr device;
        public XtChannels channels;
        public double bufferSize;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct StreamParams
    {
        public int interleaved;
        public OnBuffer onBuffer;
        public OnXRun onXRun;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct AggregateStreamParams
    {
        public StreamParams stream;
        public IntPtr devices;
        public int count;
        public XtMix mix;
        public IntPtr master;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtVersion
    {
        public int major;
        public int minor;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtLatency
    {
        public double input;
        public double output;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtBufferSize
    {
        public double min;
        public double max;
        public double current;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtBuffer
    {
        public IntPtr input;
        public IntPtr output;
        public double time;
        public ulong position;
        public ulong error;
        public int frames;
        int _timeValid;
        public bool timeValid => _timeValid != 0;
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct XtMix
    {
        public int rate;
        public XtSample sample;
        public XtMix(int rate, XtSample sample)
        => (this.rate, this.sample) = (rate, sample);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtFormat
    {
        public XtMix mix;
        public XtChannels channels;
        public XtFormat(in XtMix mix, in XtChannels channels)
        => (this.mix, this.channels) = (mix, channels);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtAttributes
    {
        public int size;
        public int count;
        int _isFloat;
        int _isSigned;
        public bool isFloat { get => _isFloat != 0; set => _isFloat = value ? 0 : 1; }
        public bool isSigned { get => _isSigned != 0; set => _isSigned = value ? 0 : 1; }
    }

    public struct XtAggregateDeviceParams
    {
        public XtDevice device;
        public XtChannels channels;
        public double bufferSize;
        public XtAggregateDeviceParams(XtDevice device, in XtChannels channels, double bufferSize)
        => (this.device, this.channels, this.bufferSize) = (device, channels, bufferSize);
    }

    public struct XtDeviceStreamParams
    {
        public XtStreamParams stream;
        public XtFormat format;
        public double bufferSize;
        public XtDeviceStreamParams(in XtStreamParams stream, in XtFormat format, double bufferSize)
        => (this.stream, this.format, this.bufferSize) = (stream, format, bufferSize);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtChannels
    {
        public int inputs;
        public ulong inMask;
        public int outputs;
        public ulong outMask;
        public XtChannels(int inputs, ulong inMask, int outputs, ulong outMask)
        => (this.inputs, this.inMask, this.outputs, this.outMask) = (inputs, inMask, outputs, outMask);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtErrorInfo
    {
        public XtSystem system;
        public XtCause cause;
        IntPtr _text;
        public int fault;
        public string text => NativeUtility.PtrToStringUTF8(_text);
        public override string ToString() => NativeUtility.PtrToStringUTF8(XtPrintErrorInfoToString(ref this));
    }

    public struct XtStreamParams
    {
        public bool interleaved;
        public XtOnBuffer onBuffer;
        public XtOnXRun onXRun;
        public XtStreamParams(bool interleaved, XtOnBuffer onBuffer, XtOnXRun onXRun)
        => (this.interleaved, this.onBuffer, this.onXRun) = (interleaved, onBuffer, onXRun);
    }

    public struct XtAggregateStreamParams
    {
        public XtStreamParams stream;
        public XtAggregateDeviceParams[] devices;
        public int count;
        public XtMix mix;
        public XtDevice master;
        public XtAggregateStreamParams(in XtStreamParams stream, XtAggregateDeviceParams[] devices, int count, in XtMix mix, XtDevice master)
        => (this.stream, this.devices, this.count, this.mix, this.master) = (stream, devices, count, mix, master);
    }
}