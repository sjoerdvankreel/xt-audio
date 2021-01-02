namespace Xt
{
    public class DeviceInfo
    {
        public string Id { get; set; }
        public string Name { get; set; }
        public XtDevice Device { get; set; }
        public bool DefaultInput { get; set; }
        public bool DefaultOutput { get; set; }
        public XtDeviceCaps Capabilities { get; set; }

        public override string ToString()
        {
            bool isDefault = DefaultInput || DefaultOutput;
            var @default = isDefault ? "(Default) " : "";
            return @default + Name + " (" + Id + ")";
        }
    }
}