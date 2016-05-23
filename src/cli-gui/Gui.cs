using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Xt {

    public partial class XtGui : Form {

        private static readonly List<int> ChannelCounts
            = Enumerable.Range(1, 64).ToList();

        private static readonly List<XtSample> Samples
            = Enum.GetValues(typeof(XtSample)).Cast<XtSample>().ToList();

        private static readonly List<int> Rates = new List<int>() {
            11025, 22050, 44100, 48000, 96000, 192000, 384000
        };

        private static readonly List<StreamType> StreamTypes
            = Enum.GetValues(typeof(StreamType)).Cast<StreamType>().ToList();

        [STAThread]
        public static void Main(string[] args) {
            Application.EnableVisualStyles();
            Application.Run(new XtGui());
        }

        private XtAudio audio;
        private XtStream inputStream;
        private XtStream outputStream;
        private FileStream captureFile;
        private ToolTip bufferTip = new ToolTip();
        private readonly List<DeviceView> deviceViews = new List<DeviceView>();

        public XtGui() {
            InitializeComponent();
        }

        private void OnStop(object sender, EventArgs e) {
            Stop();
        }

        private void OnServiceChanged(object sender, EventArgs e) {
            ServiceChanged();
        }

        private void OnDeviceChanged(object sender, EventArgs e) {
            FormatOrDeviceChanged();
        }

        private void OnFormatChanged(object sender, EventArgs e) {
            FormatOrDeviceChanged();
        }

        private void OnBufferSizeScroll(object sender, EventArgs e) {
            bufferTip.SetToolTip(bufferSize, bufferSize.Value.ToString());
        }

        private void OnTrace(XtLevel level, string message) {
            AddMessage(() => string.Format("{0}: {1}", level, message));
        }

        private void OnStreamError(Func<string> error) {
            AddMessage(error);
            BeginInvoke(new Action(() => Stop()));
        }

        private void OnShowInputPanel(object sender, EventArgs e) {
            if (inputDevice.SelectedItem != null)
                ((DeviceView)inputDevice.SelectedItem).device.ShowControlPanel();
        }

        private void OnShowOutputPanel(object sender, EventArgs e) {
            if (outputDevice.SelectedItem != null)
                ((DeviceView)outputDevice.SelectedItem).device.ShowControlPanel();
        }

        private void ClearDevices() {
            foreach (DeviceView view in deviceViews)
                view.device.Dispose();
            deviceViews.Clear();
        }

        private void OnFatal() {
            string trace = Environment.StackTrace;
            string message = "A fatal error occurred." + Environment.NewLine + "StackTrace:" + Environment.NewLine + trace;
            Invoke(new Action(() => {
                MessageBox.Show(this, message, "Fatal error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Environment.FailFast("Fatal error.");
            }));
        }

        protected override void OnClosing(CancelEventArgs e) {
            base.OnClosing(e);
            Stop();
            ClearDevices();
            audio.Dispose();
        }

        private void AddMessage(Func<string> message) {
            messages.BeginInvoke(new Action(() => {
                messages.Text += string.Format("{0} {1}{2}", DateTime.Now, message(), Environment.NewLine);
                if (messages.Text.Length > 4000)
                    messages.Text = messages.Text.Substring(messages.Text.Length - 4000);
                messages.SelectionStart = messages.TextLength;
                messages.ScrollToCaret();
            }));
        }

        private XtFormat GetFormat(bool output) {

            if (sample.SelectedItem == null
                || rate.SelectedItem == null
                || channelCount.SelectedItem == null)
                return null;

            XtFormat result = new XtFormat();
            result.mix.rate = (int)rate.SelectedItem;
            result.mix.sample = (XtSample)sample.SelectedItem;
            if (output)
                result.outputs = (int)channelCount.SelectedItem;
            else
                result.inputs = (int)channelCount.SelectedItem;
            return result;
        }

        protected override void OnShown(EventArgs e) {

            base.OnShown(e);
            audio = new XtAudio("XT-Gui", Handle, OnTrace, OnFatal);
            debug.Text = "Debug: False";
#if DEBUG
            debug.Text = "Debug: True";
#endif
            x64.Text = "X64: " + (IntPtr.Size == 8);
            isWin32.Text = "Win32: " + XtAudio.IsWin32();
            version.Text = "Version: " + XtAudio.GetVersion();

            rate.DataSource = Rates;
            rate.SelectedItem = 44100;
            sample.DataSource = Samples;
            sample.SelectedItem = XtSample.Int16;
            channelCount.DataSource = ChannelCounts;
            channelCount.SelectedItem = 2;
            streamType.DataSource = StreamTypes;
            streamType.SelectedItem = StreamType.Render;

            List<XtService> services = new List<XtService>();
            for (int i = 0; i < XtAudio.GetServiceCount(); i++)
                services.Add(XtAudio.GetServiceByIndex(i));
            service.DataSource = services;
        }

        private void ServiceChanged() {

            XtService s = (XtService)(service.SelectedItem);
            inputDevice.DataSource = null;
            outputDevice.DataSource = null;
            ClearDevices();

            DeviceView inputView = new DeviceView();
            inputView.defaultInput = true;
            inputView.device = s.OpenDefaultDevice(false);
            if (inputView.device != null)
                deviceViews.Add(inputView);

            DeviceView outputView = new DeviceView();
            outputView.defaultOutput = true;
            outputView.device = s.OpenDefaultDevice(true);
            if (outputView.device != null)
                deviceViews.Add(outputView);

            for (int i = 0; i < s.GetDeviceCount(); i++) {
                DeviceView view = new DeviceView();
                view.device = s.OpenDevice(i);
                view.index = i;
                deviceViews.Add(view);
            }

            inputDevice.DataSource = (from v in deviceViews
                                      where v.defaultInput || v.device.GetChannelCount(false) > 0
                                      select v).ToList();

            outputDevice.DataSource = (from v in deviceViews
                                       where v.defaultOutput || v.device.GetChannelCount(true) > 0
                                       select v).ToList();

            system.Text = s.GetSystem().ToString();
            capabilities.Text = XtPrint.CapabilitiesToString(s.GetCapabilities());
            defaultInput.Text = inputView.device == null ? "null" : inputView.device.ToString();
            defaultOutput.Text = outputView.device == null ? "null" : outputView.device.ToString();
            inputControlPanel.Enabled = s.GetSystem() == XtSystem.Asio;
            outputControlPanel.Enabled = s.GetSystem() == XtSystem.Asio;
        }

        private void FormatOrDeviceChanged() {

            if (sample.SelectedItem != null) {
                var attrs = XtAudio.GetSampleAttributes((XtSample)sample.SelectedItem);
                attributes.Text = XtPrint.AttributesToString(attrs);
            }

            XtFormat inputFormat = GetFormat(false);
            XtDevice inputDevice = this.inputDevice.SelectedItem == null ?
                null : ((DeviceView)(this.inputDevice.SelectedItem)).device;
            bool inputSupported = inputDevice == null ? false : inputDevice.SupportsFormat(inputFormat);
            inputFormatSupported.Text = inputSupported.ToString();
            XtBuffer inputBuffer = !inputSupported ? null : inputDevice.GetBuffer(inputFormat);
            inputBufferSizes.Text = !inputSupported ? "N/A" : string.Format("{0} / {1} / {2}",
                inputBuffer.min.ToString("N1"), inputBuffer.current.ToString("N1"), inputBuffer.max.ToString("N1"));
            inputMix.Text = inputDevice == null || inputDevice.GetMix() == null ? "N/A" : inputDevice.GetMix().ToString();
            inputInterleaved.Text = inputDevice == null
                ? "N/A"
                : inputDevice.SupportsAccess(true) && inputDevice.SupportsAccess(false)
                ? "Both"
                : inputDevice.SupportsAccess(false)
                ? "False"
                : "True";
            List<ChannelView> inputViews = new List<ChannelView>();
            if (inputDevice != null)
                inputViews = (from i in Enumerable.Range(0, inputDevice.GetChannelCount(false))
                              select new ChannelView { index = i, name = (1 + i) + ": " + inputDevice.GetChannelName(false, i) })
                              .ToList();
            inputChannels.DataSource = null;
            inputChannels.DataSource = inputViews;
            inputChannels.SelectedItems.Clear();

            XtFormat outputFormat = GetFormat(true);
            XtDevice outputDevice = this.outputDevice.SelectedItem == null ?
                null : ((DeviceView)(this.outputDevice.SelectedItem)).device;
            bool outputSupported = outputDevice == null ? false : outputDevice.SupportsFormat(outputFormat);
            outputFormatSupported.Text = outputSupported.ToString();
            XtBuffer outputBuffer = !outputSupported ? null : outputDevice.GetBuffer(outputFormat);
            outputBufferSizes.Text = !outputSupported ? "N/A" : string.Format("{0} / {1} / {2}",
                outputBuffer.min.ToString("N1"), outputBuffer.current.ToString("N1"), outputBuffer.max.ToString("N1"));
            outputMix.Text = outputDevice == null || outputDevice.GetMix() == null ? "N/A" : outputDevice.GetMix().ToString();
            outputInterleaved.Text = outputDevice == null
                ? "N/A"
                : outputDevice.SupportsAccess(true) && outputDevice.SupportsAccess(false)
                ? "Both"
                : outputDevice.SupportsAccess(false)
                ? "False"
                : "True";
            List<ChannelView> outputViews = new List<ChannelView>();
            if (outputDevice != null)
                outputViews = (from i in Enumerable.Range(0, outputDevice.GetChannelCount(true))
                               select new ChannelView { index = i, name = (1 + i) + ": " + outputDevice.GetChannelName(true, i) })
                              .ToList();
            outputChannels.DataSource = null;
            outputChannels.DataSource = outputViews;
            outputChannels.SelectedItems.Clear();

            bufferSize.Minimum = 0;
            bufferSize.Maximum = 5000;
            bufferSize.Value = 1000;
            if (outputBuffer != null) {
                bufferSize.Minimum = (int)Math.Floor(outputBuffer.min);
                bufferSize.Maximum = (int)Math.Ceiling(outputBuffer.max);
                bufferSize.Value = (int)Math.Ceiling(outputBuffer.current);
                bufferSize.TickFrequency = (bufferSize.Maximum - bufferSize.Minimum) / 10;
            }
        }

        private void Stop() {

            if (outputStream != null) {
                outputStream.Stop();
                outputStream.Dispose();
                outputStream = null;
            }

            if (inputStream != null) {
                inputStream.Stop();
                inputStream.Dispose();
                inputStream = null;
            }

            if (captureFile != null) {
                captureFile.Flush();
                captureFile.Dispose();
                captureFile = null;
            }

            stop.Enabled = false;
            panel.Enabled = true;
            start.Enabled = true;
            bufferSize.Enabled = true;
            streamType.Enabled = true;
        }

        private void OnStart(object sender, EventArgs ea) {

            try {

                StreamType type = (StreamType)streamType.SelectedItem;
                bool input = type != StreamType.Render;
                bool output = type != StreamType.Capture;

                if (input && inputDevice.SelectedItem == null) {
                    MessageBox.Show(this,
                        "Select an input device.",
                        "Invalid input device.");
                    return;
                }

                if (output && outputDevice.SelectedItem == null) {
                    MessageBox.Show(this,
                        "Select an output device.",
                        "Invalid output device.");
                    return;
                }

                XtFormat inputFormat = GetFormat(false);
                inputFormat.inputs = (int)channelCount.SelectedItem;
                if (input && inputChannels.SelectedItems.Count > 0 && inputChannels.SelectedItems.Count != inputFormat.inputs) {
                    MessageBox.Show(this,
                        "Selected either 0 input channels or a number equal to the selected format's channels.",
                        "Invalid input channel mask.");
                    return;
                }
                for (int c = 0; c < inputChannels.SelectedItems.Count; c++)
                    inputFormat.inMask |= (1UL << ((ChannelView)inputChannels.SelectedItems[c]).index);

                XtFormat outputFormat = GetFormat(true);
                if (output && outputChannels.SelectedItems.Count > 0 && outputChannels.SelectedItems.Count != outputFormat.outputs) {
                    MessageBox.Show(this,
                        "Selected either 0 output channels or a number equal to the selected format's channels.",
                        "Invalid output channel mask.");
                    return;
                }
                for (int c = 0; c < outputChannels.SelectedItems.Count; c++)
                    outputFormat.outMask |= (1UL << ((ChannelView)outputChannels.SelectedItems[c]).index);

                if (type == StreamType.Capture) {

                    captureFile = new FileStream("xt-audio.raw", FileMode.Create, FileAccess.Write);
                    CaptureCallback callback = new CaptureCallback(OnStreamError, AddMessage, captureFile);
                    XtDevice inputDevice = ((DeviceView)this.inputDevice.SelectedItem).device;
                    inputStream = inputDevice.OpenStream(inputFormat, streamInterleaved.Checked,
                        streamRaw.Checked, bufferSize.Value, callback.OnCallback, "capture-user-data");
                    callback.Init(inputStream.GetFormat(), inputStream.GetFrames());
                    inputStream.Start();

                } else if (type == StreamType.Render) {

                    RenderCallback callback = new RenderCallback(OnStreamError, AddMessage);
                    XtDevice outputDevice = ((DeviceView)this.outputDevice.SelectedItem).device;
                    outputStream = outputDevice.OpenStream(outputFormat, streamInterleaved.Checked,
                        streamRaw.Checked, bufferSize.Value, callback.OnCallback, "render-user-data");
                    outputStream.Start();

                } else if (inputDevice.SelectedItem == outputDevice.SelectedItem) {

                    XtFormat duplexFormat = inputFormat;
                    duplexFormat.outputs = outputFormat.outputs;
                    duplexFormat.outMask = outputFormat.outMask;
                    FullDuplexCallback callback = new FullDuplexCallback(OnStreamError, AddMessage);
                    XtDevice duplexDevice = ((DeviceView)this.outputDevice.SelectedItem).device;
                    outputStream = duplexDevice.OpenStream(duplexFormat, streamInterleaved.Checked,
                       streamRaw.Checked, bufferSize.Value, callback.OnCallback, "duplex-user-data");
                    outputStream.Start();

                } else {

                    double[] bufferSizes = new double[2];
                    bufferSizes[0] = bufferSize.Value;
                    bufferSizes[1] = bufferSize.Value;
                    XtDevice[] devices = new XtDevice[2];
                    devices[0] = ((DeviceView)this.inputDevice.SelectedItem).device;
                    devices[1] = ((DeviceView)this.outputDevice.SelectedItem).device;
                    XtChannels[] channels = new XtChannels[2];
                    channels[0] = new XtChannels(inputFormat.inputs, inputFormat.inMask, 0, 0);
                    channels[1] = new XtChannels(0, 0, outputFormat.outputs, outputFormat.outMask);
                    XtDevice master = outputMaster.Checked ? devices[1] : devices[0];
                    XRunCallback xRunCallback = new XRunCallback(AddMessage);
                    FullDuplexCallback streamCallback = new FullDuplexCallback(OnStreamError, AddMessage);
                    outputStream = ((XtService)service.SelectedItem).AggregateStream(
                        devices, channels, bufferSizes, 2, outputFormat.mix, streamInterleaved.Checked,
                        streamRaw.Checked, master, streamCallback.OnCallback, xRunCallback.OnCallback, "duplex-user-data");
                    outputStream.Start();
                }

                stop.Enabled = true;
                panel.Enabled = false;
                start.Enabled = false;
                bufferSize.Enabled = false;
                streamType.Enabled = false;

            } catch (XtException e) {
                Stop();
                MessageBox.Show(this, e.ToString(), "Failed to start stream.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}
