using System;
using System.Runtime.InteropServices;
using System.Security;
using static Xt.XtNative;

namespace Xt
{
	[StructLayout(LayoutKind.Sequential)]
	public struct XtVersion
	{
		public int major;
		public int minor;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct XtBuffer
	{
		public IntPtr input;
		public IntPtr output;
		public int frames;
	}

	public class XtManagedBuffer
	{
		public Array input;
		public Array output;
		public int frames;
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
	public struct XtTime
	{
		public double time;
		public ulong position;
		int _valid;
		public bool valid => _valid != 0;
	};

	[StructLayout(LayoutKind.Sequential)]
	public struct XtAttributes
	{
		public int size;
		int _isFloat;
		int _isSigned;
		public bool isFloat => _isFloat != 0;
		public bool isSigned => _isSigned != 0;
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
		public XtFormat(XtMix mix, XtChannels channels) 
			=> (this.mix, this.channels) = (mix, channels);
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

	public enum XtSetup : int { ProAudio, SystemAudio, ConsumerAudio }
	public enum XtSample : int { UInt8, Int16, Int24, Int32, Float32 }
	public enum XtCause : int { Format, Service, Generic, Unknown, Endpoint }
	public enum XtSystem : int { ALSA = 1, ASIO, JACK, WASAPI, PulseAudio, DirectSound }
	[Flags] public enum XtCapabilities : int { None = 0x0, Time = 0x1, Latency = 0x2, FullDuplex = 0x4, ChannelMask = 0x8, XRunDetection = 0x10 }

	[SuppressUnmanagedCodeSecurity]
	public delegate void XtErrorCallback(string location, string message);
	[SuppressUnmanagedCodeSecurity]
	public delegate void XtXRunCallback(int index, IntPtr user);
	public delegate void XtManagedXRunCallback(int index);
	[SuppressUnmanagedCodeSecurity]
	public delegate void XtStreamCallback(IntPtr stream, in XtBuffer buffer, in XtTime time, ulong error, IntPtr user);
	public delegate void XtManagedStreamCallback(XtStream stream, XtManagedBuffer buffer, in XtTime time, ulong error);
}