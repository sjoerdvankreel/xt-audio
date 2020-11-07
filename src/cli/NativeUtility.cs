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
}