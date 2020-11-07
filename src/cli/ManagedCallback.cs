using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace Xt
{
	class ManagedCallback
	{
		byte[] _scratch;
		XtStream _stream;
		XtManagedBuffer _buffer;
		readonly bool _interleaved;
		readonly XtManagedStreamCallback _callback;

		internal ManagedCallback(bool interleaved, XtManagedStreamCallback callback)
			=> (_interleaved, _callback) = (interleaved, callback);

		internal void Init(XtStream stream)
		{
			_stream = stream;
			var types = new Dictionary<XtSample, Type>();
			types.Add(XtSample.UInt8, typeof(byte));
			types.Add(XtSample.Int16, typeof(short));
			types.Add(XtSample.Int24, typeof(byte));
			types.Add(XtSample.Int32, typeof(int));
			types.Add(XtSample.Float32, typeof(float));
			var format = stream.GetFormat();
			var frames = stream.GetFrames();
			var type = types[format.mix.sample];
			var attrs = XtAudio.GetSampleAttributes(format.mix.sample);
			var elems = frames * attrs.count;
			var ins = format.channels.inputs;
			var outs = format.channels.outputs;
			_scratch = new byte[Math.Max(ins, outs) * frames * attrs.size];
			Func<int, Array> create = i => Array.CreateInstance(type, i);
			Func<int, IEnumerable<int>> chans = i => Enumerable.Range(0, i);
			_buffer.input = _interleaved ? create(ins * elems) : chans(ins).Select(_ => create(elems)).ToArray();
			_buffer.output = _interleaved ? create(outs * elems) : chans(outs).Select(_ => create(elems)).ToArray();
		}

		internal unsafe void Callback(IntPtr stream, in XtBuffer buffer, in XtTime time, ulong error, IntPtr user)
		{
			var format = _stream.GetFormat();
			var frames = _stream.GetFrames();
			var attrs = XtAudio.GetSampleAttributes(format.mix.sample);
			if (_interleaved && buffer.input != IntPtr.Zero)
			{
				Marshal.Copy(buffer.input, _scratch, 0, frames * format.channels.inputs * attrs.size);
				Buffer.BlockCopy(_scratch, 0, _buffer.input, 0, frames * format.channels.inputs * attrs.size);
			}
			for (int i = 0; !_interleaved && buffer.input != IntPtr.Zero && i < format.channels.inputs; i++)
			{
				IntPtr* data = (IntPtr*)buffer.input.ToPointer();
				Marshal.Copy(data[i], _scratch, 0, frames * attrs.size);
				Buffer.BlockCopy(_scratch, 0, (Array)_buffer.input.GetValue(i), 0, frames * attrs.size);
			}
			_callback(_stream, in _buffer, in time, error);
			if (_interleaved && buffer.output != IntPtr.Zero)
			{
				Buffer.BlockCopy(_buffer.output, 0, _scratch, 0, frames * format.channels.outputs * attrs.size);
				Marshal.Copy(_scratch, 0, buffer.output, frames * format.channels.outputs * attrs.size);
			}
			for (int i = 0; !_interleaved && buffer.output != IntPtr.Zero && i < format.channels.outputs; i++)
			{
				IntPtr* data = (IntPtr*)buffer.output.ToPointer();
				Buffer.BlockCopy((Array)_buffer.output.GetValue(i), 0, _scratch, 0, frames * attrs.size);
				Marshal.Copy(_scratch, 0, data[i], frames * attrs.size);
			}
		}
	}
}