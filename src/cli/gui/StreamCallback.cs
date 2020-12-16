using System;

namespace Xt
{
	abstract class StreamCallback
	{
		int processed;
		readonly bool raw;
		readonly bool interleaved;
		protected string name;
		protected readonly Action<Func<string>> onMessage;

		internal StreamCallback(bool interleaved, bool raw, string name,Action<Func<string>> onMessage)
		{
			this.raw = raw;
			this.name = name;
			this.onMessage = onMessage;
			this.interleaved = interleaved;
		}

		internal abstract void OnCallback(XtFormat format, bool interleaved,
			bool raw, object input, object output, int frames);

		internal virtual void OnMessage(Func<string> message)
		{
			onMessage.Invoke(message);
		}

		internal void OnCallback(XtStream stream, in XtBuffer buffer, object user)
		{
			XtFormat format = stream.GetFormat();
			XtSafeBuffer safe = XtSafeBuffer.Get(stream);
			if(raw)
				OnCallback(format, interleaved, raw, buffer.input, buffer.output, buffer.frames);
			else
			{
				safe.Lock(in buffer);
				OnCallback(format, interleaved, raw, safe.GetInput(), safe.GetOutput(), buffer.frames);
				safe.Unlock(in buffer);
			}
			processed += buffer.frames;
			if (processed < format.mix.rate * 3)
				return;

			processed = 0;
			XtLatency latency = stream.GetLatency();
			string formatString = "Stream {1}:{0}\tinput latency:{2}{0}\toutput latency:{3}{0}\t" +
				"buffer frames:{4}{0}\tcurrent frames:{5}{0}\ttime:{6}{0}\tposition:{7}{0}\ttimeValid:{8}{0}\tuser:{9}.";
			XtBuffer bufferLocal = buffer;
			OnMessage(() => string.Format(formatString, Environment.NewLine, name, latency.input,
				latency.output, stream.GetFrames(), bufferLocal.frames, bufferLocal.time, bufferLocal.position, bufferLocal.timeValid, user));
		}
	}
}