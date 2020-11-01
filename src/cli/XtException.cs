using System;
using static Xt.XtNative;

namespace Xt
{
	public sealed class XtException : Exception
	{
		readonly ulong _error;
		internal XtException(ulong error) => _error = error;
		public XtErrorInfo GetInfo() => XtAudioGetErrorInfo(_error);
	}
}