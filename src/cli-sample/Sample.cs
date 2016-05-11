using System;

namespace Xt {

    public static class Sample {

        [STAThread]
        public static void Main(string[] args) {

            Console.WriteLine("PrintDeviceNames:");
            PrintDeviceNames.Main(args);
            Console.WriteLine("PrintDetailedInfo:");
            PrintDetailedInfo.Main(args);
            Console.WriteLine("CaptureDefaultInput:");
            CaptureDefaultInput.Main(args);
            Console.WriteLine("RenderDefaultOutput:");
            RenderDefaultOutput.Main(args);
        }
    }
}
