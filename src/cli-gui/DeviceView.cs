namespace Xt {

    class DeviceView {

        internal XtDevice device;
        internal bool defaultInput;
        internal bool defaultOutput;
        internal int index;

        public override string ToString() {

            bool isDefault = defaultInput || defaultOutput;
            return (isDefault ? "Default: " : (index + ": ")) + device.ToString();
        }
    }
}
