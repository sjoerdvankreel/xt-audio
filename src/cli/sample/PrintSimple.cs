using System;

namespace Xt
{
    public static class PrintSimple
    {
        public static void Main()
        {
            using XtPlatform platform = XtAudio.Init(null, IntPtr.Zero, null);
            foreach (XtSystem system in platform.GetSystems())
            {
                XtService service = platform.GetService(system);
                XtDeviceList list = service.OpenDeviceList(XtEnumFlags.All);
                for (int d = 0; d < list.GetCount(); d++)
                {
                    string id = list.GetId(d);
                    Console.WriteLine(system + ": " + list.GetName(id));
                }
            }
        }
    }
}