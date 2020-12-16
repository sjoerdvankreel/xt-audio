using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Xt
{
    public partial class XtGui : Form
    {
        private static readonly List<int> ChannelCounts
            = Enumerable.Range(1, 64).ToList();

        private static readonly List<XtSample> Samples
            = Enum.GetValues(typeof(XtSample)).Cast<XtSample>().ToList();

        private static readonly List<int> Rates = new List<int>() {
            11025, 22050, 44100, 48000, 96000, 192000, 384000
        };

        private static List<StreamType> GetStreamTypes(XtService service)
        {
            var result = new List<StreamType>();
            result.Add(StreamType.Capture);
            result.Add(StreamType.Render);
            if ((service.GetCapabilities() & XtCapabilities.FullDuplex) != 0) result.Add(StreamType.Duplex);
            if ((service.GetCapabilities() & XtCapabilities.Aggregation) != 0) result.Add(StreamType.Aggregate);
            result.Add(StreamType.Latency);
            return result;
        }

        [STAThread]
        public static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.Run(new XtGui());
        }

        private TextWriter log;
        private XtPlatform platform;
        private XtStream inputStream;
        private XtStream outputStream;
        private XtSafeBuffer _safeBuffer;
        private FileStream captureFile;
        private ToolTip bufferTip = new ToolTip();
        private readonly List<DeviceView> deviceViews = new List<DeviceView>();

        public XtGui()
        {
            InitializeComponent();
        }

        private void OnStop(object sender, EventArgs e)
        {
            Stop();
        }

        private void OnSystemChanged(object sender, EventArgs e)
        {
            SystemChanged();
        }

        private void OnDeviceChanged(object sender, EventArgs e)
        {
            FormatOrDeviceChanged();
        }

        private void OnFormatChanged(object sender, EventArgs e)
        {
            FormatOrDeviceChanged();
        }

        private void OnBufferSizeScroll(object sender, EventArgs e)
        {
            bufferTip.SetToolTip(bufferSize, bufferSize.Value.ToString());
        }

        private void OnError(in XtLocation location, string message)
        {
            var loc = location;
            AddMessage(() => string.Format("{0}: {1}", loc, message));
        }

        private void OnShowInputPanel(object sender, EventArgs e)
        {
            if (inputDevice.SelectedItem != null)
                ((DeviceView)inputDevice.SelectedItem).device.ShowControlPanel();
        }

        private void OnShowOutputPanel(object sender, EventArgs e)
        {
            if (outputDevice.SelectedItem != null)
                ((DeviceView)outputDevice.SelectedItem).device.ShowControlPanel();
        }

        private void ClearDevices()
        {
            foreach (DeviceView view in deviceViews)
                view.device.Dispose();
            deviceViews.Clear();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            base.OnClosing(e);
            Stop();
            ClearDevices();
            platform.Dispose();
            log.Dispose();
        }

        private void AddMessage(Func<string> message)
        {
            string msg = message();
            log.WriteLine(msg);
            log.Flush();
            messages.BeginInvoke(new Action(() => {
                messages.Text += string.Format("{0} {1}{2}", DateTime.Now, msg, Environment.NewLine);
                if (messages.Text.Length > 4000)
                    messages.Text = messages.Text.Substring(messages.Text.Length - 4000);
                messages.SelectionStart = messages.TextLength;
                messages.ScrollToCaret();
            }));
        }

        private XtFormat? GetFormat(bool output)
        {
            if (sample.SelectedItem == null
                || rate.SelectedItem == null
                || channelCount.SelectedItem == null)
                return null;

            XtFormat result = new XtFormat();
            result.mix.rate = (int)rate.SelectedItem;
            result.mix.sample = (XtSample)sample.SelectedItem;
            if (output)
                result.channels.outputs = (int)channelCount.SelectedItem;
            else
                result.channels.inputs = (int)channelCount.SelectedItem;
            return result;
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            var libraryVersion = XtAudio.GetVersion();
            log = new StreamWriter("xt-audio.log");

            Text = $"XT-Audio {libraryVersion.major}.{libraryVersion.minor}";
            platform = XtAudio.Init("XT-Gui", Handle, OnError);
            rate.DataSource = Rates;
            rate.SelectedItem = 44100;
            sample.DataSource = Samples;
            sample.SelectedItem = XtSample.Int16;
            channelCount.DataSource = ChannelCounts;
            channelCount.SelectedItem = 2;
            system.DataSource = platform.GetSystems();
        }

        private void SystemChanged()
        {
            XtService s = platform.GetService((XtSystem)(system.SelectedItem));
            inputDevice.DataSource = null;
            outputDevice.DataSource = null;
            streamType.DataSource = GetStreamTypes(s);
            streamType.SelectedItem = StreamType.Render;
            ClearDevices();

            var inputList = s.OpenDeviceList(XtEnumFlags.Input);
            var inputViews = new List<DeviceView>();
            var defaultInputId = s.GetDefaultDeviceId(false);
            for (int i = 0; i < inputList.GetCount(); i++)
            {
                var id = inputList.GetId(i);
                var name = inputList.GetName(id);
                var view = new DeviceView();
                view.id = id;
                view.name = name;
                view.device = s.OpenDevice(id);
                view.defaultInput = id == defaultInputId;
                inputViews.Add(view);
                deviceViews.Add(view);
            }

            var outputList = s.OpenDeviceList(XtEnumFlags.Output);
            var outputViews = new List<DeviceView>();
            var defaultOutputId = s.GetDefaultDeviceId(true);
            for (int i = 0; i < outputList.GetCount(); i++)
            {
                var id = outputList.GetId(i);
                var name = outputList.GetName(id);
                var view = new DeviceView();
                view.id = id;
                view.name = name;
                view.device = s.OpenDevice(id);
                view.defaultOutput = id == defaultInputId;
                outputViews.Add(view);
                deviceViews.Add(view);
            }

            inputDevice.DataSource = new List<DeviceView>(inputViews);
            outputDevice.DataSource = new List<DeviceView>(outputViews);
            secondaryInput.DataSource = new List<DeviceView>(inputViews);
            secondaryOutput.DataSource = new List<DeviceView>(outputViews);

            capabilities.Text = s.GetCapabilities().ToString();
            defaultInput.Text = defaultInputId == null ? "null" : inputList.GetName(defaultInputId);
            defaultOutput.Text = defaultOutputId == null ? "null" : outputList.GetName(defaultOutputId);
            inputControlPanel.Enabled = (s.GetCapabilities() & XtCapabilities.ControlPanel) != 0;
            outputControlPanel.Enabled = (s.GetCapabilities() & XtCapabilities.ControlPanel) != 0;
        }

        private void FormatOrDeviceChanged()
        {
            if (sample.SelectedItem != null)
            {
                var attrs = XtAudio.GetSampleAttributes((XtSample)sample.SelectedItem);
                attributes.Text = $"Size: {attrs.size}, Count: {attrs.count}, Float: {attrs.isFloat}, Signed: {attrs.isSigned}";
            }

            XtFormat? inputFormat = GetFormat(false);
            XtDevice inputDevice = this.inputDevice.SelectedItem == null ?
                null : ((DeviceView)(this.inputDevice.SelectedItem)).device;
            bool inputSupported = inputDevice == null || inputFormat == null ? false : inputDevice.SupportsFormat(inputFormat.Value);
            inputFormatSupported.Text = inputSupported.ToString();
            XtBufferSize? inputBuffer = !inputSupported ? (XtBufferSize?)null : inputDevice.GetBufferSize(inputFormat.Value);
            inputBufferSizes.Text = !inputSupported ? "N/A" : string.Format("{0} / {1} / {2}",
                inputBuffer.Value.min.ToString("N1"), inputBuffer.Value.current.ToString("N1"), inputBuffer.Value.max.ToString("N1"));
            var inputDeviceMix = inputDevice?.GetMix();
            inputMix.Text = inputDeviceMix == null ? "N/A" : inputDeviceMix.Value.rate + " " + inputDeviceMix.Value.sample;
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

            XtFormat? outputFormat = GetFormat(true);
            XtDevice outputDevice = this.outputDevice.SelectedItem == null ?
                null : ((DeviceView)(this.outputDevice.SelectedItem)).device;
            bool outputSupported = outputDevice == null || outputFormat == null ? false : outputDevice.SupportsFormat(outputFormat.Value);
            outputFormatSupported.Text = outputSupported.ToString();
            XtBufferSize? outputBuffer = !outputSupported ? (XtBufferSize?)null : outputDevice.GetBufferSize(outputFormat.Value);
            outputBufferSizes.Text = !outputSupported ? "N/A" : string.Format("{0} / {1} / {2}",
                outputBuffer.Value.min.ToString("N1"), outputBuffer.Value.current.ToString("N1"), outputBuffer.Value.max.ToString("N1"));
            var outputDeviceMix = outputDevice?.GetMix();
            outputMix.Text = outputDeviceMix == null ? "N/A" : outputDeviceMix.Value.rate + " " + outputDeviceMix.Value.sample;
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

            bufferSize.Minimum = 1;
            bufferSize.Maximum = 5000;
            bufferSize.Value = 1000;
            if (outputBuffer != null)
            {
                bufferSize.Minimum = (int)Math.Floor(outputBuffer.Value.min);
                bufferSize.Maximum = (int)Math.Ceiling(outputBuffer.Value.max);
                bufferSize.Value = (int)Math.Ceiling(outputBuffer.Value.current);
                bufferSize.TickFrequency = (bufferSize.Maximum - bufferSize.Minimum) / 10;
            }
        }

        private void Stop()
        {
            if (outputStream != null)
            {
                outputStream.Stop();
                outputStream.Dispose();
                outputStream = null;
            }

            if (inputStream != null)
            {
                inputStream.Stop();
                inputStream.Dispose();
                inputStream = null;
            }

            if (captureFile != null)
            {
                captureFile.Flush();
                captureFile.Dispose();
                captureFile = null;
            }

            if (_safeBuffer != null)
            {
                _safeBuffer.Dispose();
                _safeBuffer = null;
            }
        }

        void OnRunning(XtStream stream, bool running, object user)
        {
            BeginInvoke(new Action(() => 
            {
                string evt = running ? "Started" : "Stopped";
                AddMessage(() => "Stream event: " + evt + ", new state: " + stream.IsRunning() + ".");
                stop.Enabled = running;
                panel.Enabled = !running;
                start.Enabled = !running;
                streamRaw.Enabled = !running;
                bufferSize.Enabled = !running;
                streamType.Enabled = !running;
                outputMaster.Enabled = !running;
                secondaryInput.Enabled = !running;
                secondaryOutput.Enabled = !running;
                streamInterleaved.Enabled = !running;
            }));
        }

        private void OnStart(object sender, EventArgs ea)
        {
            try
            {
                StreamType type = (StreamType)streamType.SelectedItem;
                bool input = type == StreamType.Capture || type == StreamType.Duplex || type == StreamType.Latency;
                bool output = type == StreamType.Render || type == StreamType.Duplex || type == StreamType.Latency;
                var inputView = (DeviceView)this.inputDevice.SelectedItem;
                var outputView = (DeviceView)this.outputDevice.SelectedItem;
                XtDevice inputDevice = inputView.device;
                XtDevice outputDevice = outputView.device;
                XtDevice secondaryInputDevice = ((DeviceView)this.secondaryInput.SelectedItem).device;
                XtDevice secondaryOutputDevice = ((DeviceView)this.secondaryOutput.SelectedItem).device;

                if (input && inputDevice == null)
                {
                    MessageBox.Show(this,
                        "Select an input device.",
                        "Invalid input device.");
                    return;
                }

                if (output && outputDevice == null)
                {
                    MessageBox.Show(this,
                        "Select an output device.",
                        "Invalid output device.");
                    return;
                }

                if (type == StreamType.Duplex && inputView.id != outputView.id)
                {
                    MessageBox.Show(this,
                        "For duplex operation, input and output device must be the same.",
                        "Invalid duplex device.");
                    return;
                }

                if (type == StreamType.Aggregate
                    && (inputDevice == null && secondaryInputDevice == null
                     || outputDevice == null && secondaryOutputDevice == null))
                {
                    MessageBox.Show(this,
                        "For aggregate operation, select at least 1 input and 1 output device.",
                        "Invalid aggregate device.");
                    return;
                }

                XtSystem system = (XtSystem)this.system.SelectedItem;
                OnXRun onXRunWrapper = new OnXRun(AddMessage);
                bool doLogXRuns = logXRuns.CheckState == CheckState.Checked || (logXRuns.CheckState == CheckState.Indeterminate && system != XtSystem.JACK);
                XtOnXRun onXRun = doLogXRuns ? onXRunWrapper.OnCallback : (XtOnXRun)null;

                XtFormat inputFormat = GetFormat(false).Value;
                inputFormat.channels.inputs = (int)channelCount.SelectedItem;
                if (input && inputChannels.SelectedItems.Count > 0 && inputChannels.SelectedItems.Count != inputFormat.channels.inputs)
                {
                    MessageBox.Show(this,
                        "Selected either 0 input channels or a number equal to the selected format's channels.",
                        "Invalid input channel mask.");
                    return;
                }
                for (int c = 0; c < inputChannels.SelectedItems.Count; c++)
                    inputFormat.channels.inMask |= (1UL << ((ChannelView)inputChannels.SelectedItems[c]).index);

                XtFormat outputFormat = GetFormat(true).Value;
                if (output && outputChannels.SelectedItems.Count > 0 && outputChannels.SelectedItems.Count != outputFormat.channels.outputs)
                {
                    MessageBox.Show(this,
                        "Selected either 0 output channels or a number equal to the selected format's channels.",
                        "Invalid output channel mask.");
                    return;
                }
                for (int c = 0; c < outputChannels.SelectedItems.Count; c++)
                    outputFormat.channels.outMask |= (1UL << ((ChannelView)outputChannels.SelectedItems[c]).index);

                if (type == StreamType.Capture)
                {
                    captureFile = new FileStream("xt-audio.raw", FileMode.Create, FileAccess.Write);
                    CaptureCallback callback = new CaptureCallback(streamInterleaved.Checked, streamRaw.Checked, AddMessage, captureFile);
                    var streamParams = new XtStreamParams(streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    var deviceParams = new XtDeviceStreamParams(in streamParams, in inputFormat, bufferSize.Value);
                    inputStream = inputDevice.OpenStream(in deviceParams, "capture-user-data");
                    callback.Init(inputStream.GetFormat(), inputStream.GetFrames());
                    _safeBuffer = XtSafeBuffer.Register(inputStream, streamInterleaved.Checked);
                    inputStream.Start();
                } else if (type == StreamType.Render)
                {
                    RenderCallback callback = new RenderCallback(streamInterleaved.Checked, streamRaw.Checked, AddMessage);
                    var streamParams = new XtStreamParams(streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    var deviceParams = new XtDeviceStreamParams(in streamParams, in outputFormat, bufferSize.Value);
                    outputStream = outputDevice.OpenStream(in deviceParams, "render-user-data");
                    _safeBuffer = XtSafeBuffer.Register(outputStream, streamInterleaved.Checked);
                    outputStream.Start();
                } else if (type == StreamType.Duplex)
                {
                    XtFormat duplexFormat = inputFormat;
                    duplexFormat.channels.outputs = outputFormat.channels.outputs;
                    duplexFormat.channels.outMask = outputFormat.channels.outMask;
                    FullDuplexCallback callback = new FullDuplexCallback(streamInterleaved.Checked, streamRaw.Checked, AddMessage);
                    var streamParams = new XtStreamParams(streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    var deviceParams = new XtDeviceStreamParams(in streamParams, in duplexFormat, bufferSize.Value);
                    outputStream = outputDevice.OpenStream(in deviceParams, "duplex-user-data");
                    _safeBuffer = XtSafeBuffer.Register(outputStream, streamInterleaved.Checked);
                    outputStream.Start();
                } else if (type == StreamType.Aggregate)
                {
                    var devices = new List<XtAggregateDeviceParams>();
                    if (inputDevice != null) devices.Add(new XtAggregateDeviceParams(inputDevice, new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0), bufferSize.Value));
                    if (outputDevice != null) devices.Add(new XtAggregateDeviceParams(outputDevice, new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask), bufferSize.Value));
                    if (secondaryInputDevice != null) devices.Add(new XtAggregateDeviceParams(secondaryInputDevice, new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0), bufferSize.Value));
                    if (secondaryOutputDevice != null) devices.Add(new XtAggregateDeviceParams(secondaryOutputDevice, new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask), bufferSize.Value));

                    XtDevice master = outputMaster.Checked ?
                        (outputDevice != null ? outputDevice :
                        secondaryOutputDevice != null ? secondaryOutputDevice :
                        inputDevice != null ? inputDevice : secondaryInputDevice) :
                        (inputDevice != null ? inputDevice :
                        secondaryInputDevice != null ? secondaryInputDevice :
                        outputDevice != null ? outputDevice : secondaryOutputDevice);

                    AggregateCallback streamCallback = new AggregateCallback(streamInterleaved.Checked, streamRaw.Checked, AddMessage);
                    var streamParams = new XtStreamParams(streamInterleaved.Checked, streamCallback.OnCallback, onXRun, OnRunning);
                    var aggregateParams = new XtAggregateStreamParams(in streamParams, devices.ToArray(), devices.Count, outputFormat.mix, master);
                    outputStream = platform.GetService(((XtSystem)this.system.SelectedItem)).AggregateStream(in aggregateParams, "aggregate-user-data");
                    streamCallback.Init(outputStream.GetFrames());
                    _safeBuffer = XtSafeBuffer.Register(outputStream, streamInterleaved.Checked);
                    outputStream.Start();
                } else if (inputDevice == outputDevice)
                {
                    XtFormat duplexFormat = inputFormat;
                    duplexFormat.channels.outputs = outputFormat.channels.outputs;
                    duplexFormat.channels.outMask = outputFormat.channels.outMask;
                    LatencyCallback callback = new LatencyCallback(streamInterleaved.Checked, streamRaw.Checked, AddMessage);
                    var streamParams = new XtStreamParams(streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    var deviceParams = new XtDeviceStreamParams(in streamParams, in duplexFormat, bufferSize.Value);
                    outputStream = outputDevice.OpenStream(in deviceParams, "latency-user-data");
                    _safeBuffer = XtSafeBuffer.Register(outputStream, streamInterleaved.Checked);
                    outputStream.Start();
                } else
                {
                    XtDevice master = outputMaster.Checked ? outputDevice : inputDevice;
                    XtAggregateDeviceParams[] devices = new XtAggregateDeviceParams[2];
                    devices[0] = new XtAggregateDeviceParams(inputDevice, new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0), bufferSize.Value);
                    devices[1] = new XtAggregateDeviceParams(outputDevice, new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask), bufferSize.Value);
                    LatencyCallback callback = new LatencyCallback(streamInterleaved.Checked, streamRaw.Checked, AddMessage);
                    XtStreamParams streamParams = new XtStreamParams(streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    XtAggregateStreamParams aggregateParams = new XtAggregateStreamParams(in streamParams, devices, 2, in outputFormat.mix, master);
                    outputStream = platform.GetService(((XtSystem)this.system.SelectedItem)).AggregateStream(in aggregateParams, "latency-user-data");
                    _safeBuffer = XtSafeBuffer.Register(outputStream, streamInterleaved.Checked);
                    outputStream.Start();
                }

            } catch (XtException e)
            {
                Stop();
                MessageBox.Show(this, e.ToString(), "Failed to start stream.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}