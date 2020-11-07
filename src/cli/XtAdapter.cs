using System;

namespace Xt
{
	public class XtAdapter : IDisposable
	{
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
		public static XtAdapter Register(XtStream stream, object user) => new XtAdapter(stream, user);
	}
}