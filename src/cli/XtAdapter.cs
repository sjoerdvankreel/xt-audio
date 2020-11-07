using System;
using System.Collections.Generic;

namespace Xt
{
	public struct XtBufferAdapter
	{
		public Array input;
		public Array output;
	}

	public class XtAdapter : IDisposable
	{
		static readonly Dictionary<IntPtr, XtAdapter> _map = new Dictionary<IntPtr, XtAdapter>();

		readonly object _user;
		readonly XtStream _stream;
		internal XtAdapter(XtStream stream, object user) => (_stream, _user) = (stream, user);

		public void Dispose() { }
		public Array Input => null;
		public Array Output => null;
		public object User => _user;
		public IDisposable IO() => null;
		public XtStream Stream => _stream;
		public static XtAdapter Get(IntPtr stream) => null;

		public static XtAdapter Register(XtStream stream, object user)
		{
			var result = new XtAdapter(stream, user);
			_map.Add(stream.Handle, result);
			return result;
		}
	}
}