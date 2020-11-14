using System;

namespace Xt
{
    public class Sample
    {
        static readonly Action[] Samples =
        {
            PrintSimple.Main, PrintDetailed.Main, CaptureSimple.Main, RenderSimple.Main,
            CaptureAdvanced.Main, RenderAdvanced.Main, FullDuplex.Main, Aggregate.Main
         };

        static readonly string[] Names =
        {
            nameof(PrintSimple), nameof(PrintDetailed), nameof(CaptureSimple), nameof(RenderSimple),
            nameof(CaptureAdvanced), nameof(RenderAdvanced), nameof(FullDuplex), nameof(Aggregate)
        };

        static void RunSample(int index)
        {
            Console.WriteLine(Names[index] + ":");
            Samples[index]();
        }

        [STAThread]
        public static void Main(string[] args)
        {
            int index = args.Length == 1 ? int.Parse(args[0]) : -1;
            try
            {
                if (index >= 0)
                    RunSample(index);
                else
                    for (int i = 0; i < Samples.Length; i++)
                        RunSample(i);
            } catch (XtException e)
            { Console.WriteLine(XtAudio.GetErrorInfo(e.GetError())); }
        }
    }
}