using System;
using System.Collections.Generic;

namespace Xt
{
    public static class XtPrint
    {
        public static string LevelToString(XtLevel level) => XtNative.StringFromUtf8(XtNative.XtPrintLevelToString(level));
        public static string CauseToString(XtCause cause) => XtNative.StringFromUtf8(XtNative.XtPrintCauseToString(cause));
        public static string SetupToString(XtSetup setup) => XtNative.StringFromUtf8(XtNative.XtPrintSetupToString(setup));
        public static string SystemToString(XtSystem system) => XtNative.StringFromUtf8(XtNative.XtPrintSystemToString(system));
        public static string SampleToString(XtSample sample) => XtNative.StringFromUtf8(XtNative.XtPrintSampleToString(sample));
        
        public static unsafe IList<string> CapabilitiesToString(XtCapabilities capabilities)
		{
            int i = 0;
            var result = new List<string>();
            var strings = (IntPtr*)XtNative.XtPrintCapabilitiesToString(capabilities);
            while (strings[i] != IntPtr.Zero)
                result.Add(XtNative.StringFromUtf8(strings[i++]));
            return result;
        }
    }
}