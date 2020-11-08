using System;
using System.Collections.Generic;

namespace Xt
{
	public class XtAdapter : IDisposable
	{
		static readonly Dictionary<IntPtr, XtAdapter> _map = new Dictionary<IntPtr, XtAdapter>();

		readonly object _user;
		readonly Array _input;
		readonly Array _output;
		readonly XtStream _stream;

		public object GetUser() => _user;
		public Array GetInput() => _input;
		public Array GetOutput() => _output;
		public XtStream GetStream() => _stream;
		public void Dispose() => _map.Remove(_stream.Handle);
		public static XtAdapter Get(IntPtr stream) => _map[stream];

		public static XtAdapter Register(XtStream stream, bool interleaved, object user)
		{
			var result = new XtAdapter(stream, interleaved, user);
			_map.Add(stream.Handle, result);
			return result;
		}

		internal XtAdapter(XtStream stream, bool interleaved, object user)
		{
			_user = user;
			_stream = stream;
			var format = stream.GetFormat();
			var frames = stream.GetFrames();
			_input = CreateBuffer(format.mix.sample, format.channels.inputs, interleaved, frames);
			_output = CreateBuffer(format.mix.sample, format.channels.outputs, interleaved, frames);
		}

		public void Lock() { }
		public void Unlock() { }

		static Array CreateBuffer(XtSample sample, int channels, bool interleaved, int frames)
		{
		}
	}
}