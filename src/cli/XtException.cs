using System;

namespace Xt
{
	public sealed class XtException : Exception
	{
		private readonly ulong error;
		public ulong GetError() => error;
		internal XtException(ulong error) => this.error = error;
	}
}