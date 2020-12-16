using System;
using System.Runtime.InteropServices;
using static Xt.Utility;

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
        public OnRunning onRunning;
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
        public int frames;
        int _timeValid;
        public bool timeValid => _timeValid != 0;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtServiceError
    {
        public XtCause cause;
        IntPtr _text;
        public string text => PtrToStringUTF8(_text);
    }

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

    [StructLayout(LayoutKind.Sequential)]
    public struct XtLocation
    {
        IntPtr _file;
        public string file => PtrToStringUTF8(_file);
        IntPtr _func;
        public string func => PtrToStringUTF8(_func);
        public int line;
        public override string ToString() => PtrToStringUTF8(XtPrintLocationToString(ref this));
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct XtErrorInfo
    {
        public int fault;
        public XtSystem system;
        public XtServiceError service;        
        public override string ToString() => PtrToStringUTF8(XtPrintErrorInfoToString(ref this));
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

    public struct XtStreamParams
    {
        public bool interleaved;
        public XtOnBuffer onBuffer;
        public XtOnXRun onXRun;
        public XtOnRunning onRunning;
        public XtStreamParams(bool interleaved, XtOnBuffer onBuffer, XtOnXRun onXRun, XtOnRunning onRunning)
        => (this.interleaved, this.onBuffer, this.onXRun, this.onRunning) = (interleaved, onBuffer, onXRun, onRunning);
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