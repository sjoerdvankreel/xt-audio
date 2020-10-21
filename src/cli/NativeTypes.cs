using System;
using System.Runtime.InteropServices;

namespace Xt
{
	public enum XtLevel : int { Info, Error, Fatal }
	public enum XtSample : int { UInt8, Int16, Int24, Int32, Float32 }
	public enum XtSetup : int { ProAudio, SystemAudio, ConsumerAudio }
	public enum XtCause : int { Format, Service, Generic, Unknown, Endpoint }
	public enum XtSystem : int { Alsa = 1, Asio, Jack, Pulse, DSound, Wasapi }
	[Flags] public enum XtCapabilities : int { None = 0x0, Time = 0x1, Latency = 0x2, FullDuplex = 0x4, ChannelMask = 0x8, XRunDetection = 0x10 }

	public delegate void XtFatalCallback();
	public delegate void XtXRunCallback(int index, object user);
	public delegate void XtTraceCallback(XtLevel level, string message);
	public delegate void XtStreamCallback(XtStream stream, object input, object output, int frames, double time, ulong position, bool timeValid, ulong error, object user);

	[StructLayout(LayoutKind.Sequential)]
	public struct XtLatency
	{
		public double input;
		public double output;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct XtBuffer
	{
		public double min;
		public double max;
		public double current;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct XtMix
	{
		public int rate;
		public XtSample sample;

		public XtMix(int rate, XtSample sample)
		{
			this.rate = rate;
			this.sample = sample;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct XtFormat
	{
		public XtMix mix;
		public XtChannels channels;

		public XtFormat(XtMix mix, XtChannels channels)
		{
			this.mix = mix;
			this.channels = channels;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct XtChannels
	{
		public int inputs;
		public ulong inMask;
		public int outputs;
		public ulong outMask;

		public XtChannels(int inputs, ulong inMask, int outputs, ulong outMask)
		{
			this.inputs = inputs;
			this.inMask = inMask;
			this.outputs = outputs;
			this.outMask = outMask;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct XtAttributes
	{
		public int size;
		int _isFloat;
		int _isSigned;
		public bool isFloat { get => _isFloat != 0; set => _isFloat = value ? 0 : 1; }
		public bool isSigned { get => _isSigned != 0; set => _isSigned = value ? 0 : 1; }
	}
}