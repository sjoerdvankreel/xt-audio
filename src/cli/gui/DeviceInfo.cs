namespace Xt
{
    class DeviceInfo
    {
        internal XtDevice device;
        internal string id;
        internal string name;
        internal bool defaultInput;
        internal bool defaultOutput;
        internal XtDeviceCaps capabilities;

        public override string ToString()
        {
            bool isDefault = defaultInput || defaultOutput;
            var @default = isDefault ? "(Default) " : "";
            return @default + name + " (" + id + ")";
        }
    }
}