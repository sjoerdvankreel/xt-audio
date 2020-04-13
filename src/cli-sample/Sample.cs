using System;

namespace Xt
{
    public class Sample
    {
        [STAThread]
        public static void Main(string[] args)
        {
            Console.WriteLine("PrintSimple:");
            PrintSimple.Main(args);
            Console.WriteLine("PrintDetailed:");
            PrintDetailed.Main(args);
            Console.WriteLine("CaptureSimple:");
            CaptureSimple.Main(args);
            Console.WriteLine("RenderSimple:");
            RenderSimple.Main(args);
            Console.WriteLine("CaptureAdvanced:");
            CaptureAdvanced.Main(args);
            Console.WriteLine("RenderAdvanced:");
            RenderAdvanced.Main(args);
            Console.WriteLine("FullDuplex:");
            FullDuplex.Main(args);
            Console.WriteLine("Aggregate:");
            Aggregate.Main(args);
        }
    }
}