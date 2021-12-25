using System;

namespace Xt
{
	public sealed class XtException : Exception
	{
		readonly ulong _error;
		public ulong GetError() => _error;
		internal XtException(ulong error) => _error = error;
		public override string Message => XtAudio.GetErrorInfo(GetError()).ToString();
	}
}