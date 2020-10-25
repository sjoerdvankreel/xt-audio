using System;

namespace Xt
{
	public sealed class XtException : Exception
	{
		private readonly ulong _error;
		public ulong GetError() => _error;
		internal XtException(ulong error) => _error = error;
	}
}