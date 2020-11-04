using System;
using System.Linq;
using System.Text;

namespace Xt
{
	static class NativeUtility
	{
		internal static unsafe string PtrToStringUTF8(IntPtr ptr)
		{
			var range = Enumerable.Range(0, int.MaxValue);
			var length = range.SkipWhile(i => ((byte*)ptr)[i] != 0).First();
			return Encoding.UTF8.GetString((byte*)ptr, length);
		}
	}

	class XRunCallbackWrapper
	{
		readonly XtManagedXRunCallback _callback;
		internal void Callback(int index, IntPtr user) => _callback(index);
		internal XRunCallbackWrapper(XtManagedXRunCallback callback) => _callback = callback;
	}

	class StreamCallbackWrapper
	{
		internal XtStream _stream;
		readonly XtManagedStreamCallback _callback;
		internal StreamCallbackWrapper(XtManagedStreamCallback callback) => _callback = callback;
		internal void Callback(IntPtr stream, in XtBuffer buffer, in XtTime time, ulong error, IntPtr user) => _callback(_stream, null, in time, error);
	}
}