namespace Xt
{
    public static class XtPrint
    {
        public static string LevelToString(XtLevel level) => XtNative.StringFromUtf8(XtNative.XtPrintLevelToString(level));
        public static string CauseToString(XtCause cause) => XtNative.StringFromUtf8(XtNative.XtPrintCauseToString(cause));
        public static string SetupToString(XtSetup setup) => XtNative.StringFromUtf8(XtNative.XtPrintSetupToString(setup));
        public static string SystemToString(XtSystem system) => XtNative.StringFromUtf8(XtNative.XtPrintSystemToString(system));
        public static string SampleToString(XtSample sample) => XtNative.StringFromUtf8(XtNative.XtPrintSampleToString(sample));

        public static string MixToString(XtMix mix) => XtNative.FreeStringFromUtf8(XtNative.XtPrintMixToString(mix));
        public static string ErrorToString(ulong error) => XtNative.FreeStringFromUtf8(XtNative.XtPrintErrorToString(error));
        public static string BufferToString(XtBuffer buffer) => XtNative.FreeStringFromUtf8(XtNative.XtPrintBufferToString(buffer));
        public static string LatencyToString(XtLatency latency) => XtNative.FreeStringFromUtf8(XtNative.XtPrintLatencyToString(latency));
        public static string ChannelsToString(XtChannels channels) => XtNative.FreeStringFromUtf8(XtNative.XtPrintChannelsToString(channels));
        public static string AttributesToString(XtAttributes attributes) => XtNative.FreeStringFromUtf8(XtNative.XtPrintAttributesToString(attributes));
        public static string CapabilitiesToString(XtCapabilities capabilities) => XtNative.FreeStringFromUtf8(XtNative.XtPrintCapabilitiesToString(capabilities));

        public static string FormatToString(XtFormat format)
        {
            XtNative.Format native = XtNative.Format.ToNative(format);
            return XtNative.FreeStringFromUtf8(XtNative.XtPrintFormatToString(ref native));
        }
    }
}