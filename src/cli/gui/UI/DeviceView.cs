using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Xt
{
    public partial class DeviceView : UserControl
    {
        public DeviceView() => InitializeComponent();

        static string FormatBuffer(XtBufferSize size)
        => string.Format("{0} / {1} / {2}", size.min.ToString("N1"), size.current.ToString("N1"), size.max.ToString("N1"));

        static string FormatAccess(XtDevice device)
        {
            bool interleaved = device.SupportsAccess(true);
            bool nonInterleaved = device.SupportsAccess(false);
            return interleaved && nonInterleaved ? "Both" : interleaved ? "True" : "False";
        }

        void OnControlPanelClick(object sender, EventArgs e)
        => (_device.SelectedItem as DeviceInfo)?.Device?.ShowControlPanel();

        public void SystemChanged(XtService s, IList<DeviceInfo> devices)
        {
            _device.DataSource = null;
            _device.DataSource = devices;
            _device.SelectedIndex = devices.Count == 1 ? 0 : 1;
            _controlPanel.Enabled = (s.GetCapabilities() & XtServiceCaps.ControlPanel) != 0;
        }

        public void FormatOrDeviceChanged(bool output, XtFormat? format)
        {
            DeviceInfo info = _device.SelectedItem as DeviceInfo;
            XtDevice device = info?.Device;
            bool supported = format != null && device?.SupportsFormat(format.Value) == true;
            _formatSupported.Text = supported.ToString();
            XtBufferSize? buffer = !supported ? (XtBufferSize?)null : device.GetBufferSize(format.Value);
            _bufferSizes.Text = !supported ? "N/A" : FormatBuffer(buffer.Value);
            var mix = device?.GetMix();
            _mix.Text = mix == null ? "N/A" : mix.Value.rate + " " + mix.Value.sample;
            _interleaved.Text = device == null ? "N/A" : FormatAccess(device);
            _capabilities.Text = info == null ? "None" : info.Capabilities.ToString();
            var channels = new List<ChannelInfo>();
            if (device != null) channels = Enumerable.Range(0, device.GetChannelCount(output))
                    .Select(i => new ChannelInfo(i, (1 + i) + ": " + device.GetChannelName(output, i)))
                    .ToList();
            _channels.DataSource = null;
            _channels.DataSource = channels;
            _channels.SelectedItems.Clear();
        }
    }
}