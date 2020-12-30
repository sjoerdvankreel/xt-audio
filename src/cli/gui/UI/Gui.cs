using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows.Forms;

namespace Xt
{
    public partial class XtGui : Form
    {
        const int MaxMessages = 10;
        static readonly IList<int> ChannelCounts = Enumerable.Range(1, 64).ToList();
        static readonly IList<int> Rates = new List<int>() { 11025, 22050, 44100, 48000, 96000, 192000 };
        static readonly IList<XtSample> Samples = Enum.GetValues(typeof(XtSample)).Cast<XtSample>().ToList();

        static void OnThreadException(object sender, ThreadExceptionEventArgs e)
        => OnError(e.Exception);
        static void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
        => OnError((Exception)e.ExceptionObject);

        [STAThread]
        public static void Main(string[] args)
        {
            Application.ThreadException += OnThreadException;
            AppDomain.CurrentDomain.UnhandledException += OnUnhandledException;
            Application.EnableVisualStyles();
            Application.Run(new XtGui());
        }

        static void OnError(Exception e)
        {
            string message = e.ToString();
            if (e is XtException xt)
                message = XtAudio.GetErrorInfo(xt.GetError()).ToString();
            MessageBox.Show(message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        static IList<StreamType> GetStreamTypes(XtService service)
        {
            var result = new List<StreamType>();
            result.Add(StreamType.Render);
            result.Add(StreamType.Capture);
            if ((service.GetCapabilities() & XtServiceCaps.FullDuplex) != 0) result.Add(StreamType.Duplex);
            if ((service.GetCapabilities() & XtServiceCaps.Aggregation) != 0) result.Add(StreamType.Aggregate);
            result.Add(StreamType.Latency);
            return result;
        }

        TextWriter _log;
        XtPlatform _platform;
        XtStream _inputStream;
        XtStream _outputStream;
        FileStream _captureFile;
        XtSafeBuffer _safeBuffer;

        bool _messageAdded;
        System.Windows.Forms.Timer _timer;
        ToolTip _bufferTip = new ToolTip();
        IList<string> _messages = new List<string>();
        IList<DeviceInfo> _allDevices = new List<DeviceInfo>();

        void OnStop(object sender, EventArgs e) => Stop();
        void OnDeviceChanged(object sender, EventArgs e) => FormatOrDeviceChanged();
        void OnFormatChanged(object sender, EventArgs e) => FormatOrDeviceChanged();

        void OnError(in XtLocation location, string message)
        => AddError(location, message);
        void AddError(XtLocation location, string message)
        => AddMessage(() => string.Format("{0}: {1}", location, message));
        void OnBufferSizeScroll(object sender, EventArgs e)
        => _bufferTip.SetToolTip(_bufferSize, _bufferSize.Value.ToString());

        public XtGui()
        {
            InitializeComponent();
            _timer = new System.Windows.Forms.Timer();
            _timer.Interval = 1000;
            _timer.Tick += OnTimerTick;
            _timer.Start();
        }

        void Stop()
        {
            _inputStream?.Stop();
            _inputStream?.Dispose();
            _outputStream?.Stop();
            _outputStream?.Dispose();
            _captureFile?.Flush();
            _captureFile?.Dispose(); 
            _safeBuffer?.Dispose();
        }

        void ClearDevices()
        {
            foreach (DeviceInfo info in _allDevices)
                info.Device.Dispose();
            _allDevices.Clear();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            base.OnClosing(e);
            Stop();
            ClearDevices();
            _platform.Dispose();
            _log.Dispose();
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            var version = XtAudio.GetVersion();
            _log = new StreamWriter($"xt-audio.log");
            Text = $"XT-Audio {version.major}.{version.minor}";
            _platform = XtAudio.Init("XT-Gui", Handle, OnError);
            _rate.DataSource = Rates;
            _rate.SelectedItem = 48000;
            _sample.DataSource = Samples;
            _sample.SelectedItem = XtSample.Int16;
            _channelCount.DataSource = ChannelCounts;
            _channelCount.SelectedItem = 2;
            _system.DataSource = _platform.GetSystems();
        }

        void OnTimerTick(object sender, EventArgs e)
        {
            if (!_messageAdded) return;
            while (_messages.Count > MaxMessages) _messages.RemoveAt(0);
            _guiLog.Text = string.Join(Environment.NewLine, _messages);
            _guiLog.SelectionStart = _guiLog.TextLength;
            _guiLog.ScrollToCaret();
            _messageAdded = false;
        }

        void AddMessage(Func<string> message)
        {
            string msg = message();
            _log.WriteLine(msg);
            _log.Flush();
            _guiLog.BeginInvoke(new Action(() => {
                _messages.Add(string.Format("{0}: {1}", DateTime.Now, msg));
                _messageAdded = true;
            }));
        }

        XtFormat? GetFormat(bool output)
        {
            if (_sample.SelectedItem == null
                || _rate.SelectedItem == null
                || _channelCount.SelectedItem == null) return null;
            XtFormat result = new XtFormat();
            result.mix.rate = (int)_rate.SelectedItem;
            result.mix.sample = (XtSample)_sample.SelectedItem;
            if (output) result.channels.outputs = (int)_channelCount.SelectedItem;
            else result.channels.inputs = (int)_channelCount.SelectedItem;
            return result;
        }

        DeviceInfo GetDeviceInfo(XtService service, XtDeviceList list, int index, string defaultId)
        {
            var id = list.GetId(index);
            var result = new DeviceInfo();
            result.Id = id;
            result.Name = list.GetName(id);
            result.DefaultInput = id == defaultId;
            result.DefaultOutput = id == defaultId;
            result.Device = service.OpenDevice(id);
            result.Capabilities = list.GetCapabilities(id);
            return result;
        }

        IList<DeviceInfo> GetDeviceInfos(XtService service, XtDeviceList list, string defaultId)
        {
            var result = new List<DeviceInfo>();
            var noDevice = new DeviceInfo();
            noDevice.Id = "None";
            noDevice.Name = "[None]";
            result.Add(noDevice);
            for (int i = 0; i < list.GetCount(); i++)
                try
                {
                    var info = GetDeviceInfo(service, list, i, defaultId);
                    result.Insert(info.DefaultInput || info.DefaultInput ? 1 : result.Count, info);
                    _allDevices.Add(info);
                } catch (XtException e)
                {
                    AddMessage(() => XtAudio.GetErrorInfo(e.GetError()).ToString());
                }
            return result;
        }

        void OnSystemChanged(object sender, EventArgs e)
        {
            XtService s = _platform.GetService((XtSystem)_system.SelectedItem);
            _streamType.DataSource = GetStreamTypes(s);
            _streamType.SelectedItem = StreamType.Render;
            ClearDevices();
            var defaultInputId = s.GetDefaultDeviceId(false);
            var defaultOutputId = s.GetDefaultDeviceId(true);
            using var inputList = s.OpenDeviceList(XtEnumFlags.Input);
            using var outputList = s.OpenDeviceList(XtEnumFlags.Output);
            var inputs = GetDeviceInfos(s, inputList, defaultInputId);
            var outputs = GetDeviceInfos(s, outputList, defaultOutputId);
            _input.SystemChanged(s, inputs);
            _output.SystemChanged(s, outputs);
            _serviceCaps.Text = s.GetCapabilities().ToString();
            _secondaryInput.DataSource = new List<DeviceInfo>(inputs);
            _secondaryOutput.DataSource = new List<DeviceInfo>(outputs);
            _defaultInput.Text = defaultInputId == null ? "[None]" : inputList.GetName(defaultInputId);
            _defaultOutput.Text = defaultOutputId == null ? "[None]" : outputList.GetName(defaultOutputId);
        }

        void FormatOrDeviceChanged()
        {
            _bufferSize.Minimum = 1;
            _bufferSize.Value = 1000;
            _bufferSize.Maximum = 5000;
            _output.FormatOrDeviceChanged(true, GetFormat(true));
            _input.FormatOrDeviceChanged(false, GetFormat(false));

            if (_sample.SelectedItem != null)
            {
                var attrs = XtAudio.GetSampleAttributes((XtSample)_sample.SelectedItem);
                _attributes.Text = $"Size: {attrs.size}, Count: {attrs.count}, Float: {attrs.isFloat}, Signed: {attrs.isSigned}";
            }

            var format = GetFormat(true);
            var buffer = format == null ? null : (_output._device.SelectedItem as DeviceInfo)?.Device?.GetBufferSize(format.Value);
            if (buffer != null)
            {
                _bufferSize.Minimum = (int)Math.Floor(buffer.Value.min);
                _bufferSize.Maximum = (int)Math.Ceiling(buffer.Value.max);
                _bufferSize.Value = (int)Math.Ceiling(buffer.Value.current);
                _bufferSize.TickFrequency = (_bufferSize.Maximum - _bufferSize.Minimum) / 10;
            }
        }

        void OnRunning(XtStream stream, bool running, ulong error, object user)
        {
            bool evt = running;
            bool newState = stream.IsRunning();
            BeginInvoke(new Action(() => {
                string evtDesc = running ? "Started" : "Stopped";
                AddMessage(() => "Stream event: " + evtDesc + ", new state: " + newState + ".");
                if (error != 0) AddMessage(() => XtAudio.GetErrorInfo(error).ToString());
                _stop.Enabled = running;
                panel.Enabled = !running;
                _start.Enabled = !running;
                _bufferSize.Enabled = !running;
                _streamType.Enabled = !running;
                _streamNative.Enabled = !running;
                _outputMaster.Enabled = !running;
                _secondaryInput.Enabled = !running;
                _secondaryOutput.Enabled = !running;
                _streamInterleaved.Enabled = !running;
            }));
        }

        private void OnStart(object sender, EventArgs ea)
        {
            try
            {
                StreamType type = (StreamType)_streamType.SelectedItem;
                bool input = type == StreamType.Capture || type == StreamType.Duplex || type == StreamType.Latency;
                bool output = type == StreamType.Render || type == StreamType.Duplex || type == StreamType.Latency;
                var inputInfo = (DeviceInfo)this._inputDevice.SelectedItem;
                var outputInfo = (DeviceInfo)this._outputDevice.SelectedItem;
                XtDevice inputDevice = inputInfo.Device;
                XtDevice outputDevice = outputInfo.Device;
                XtDevice secondaryInputDevice = ((DeviceInfo)this._secondaryInput.SelectedItem).Device;
                XtDevice secondaryOutputDevice = ((DeviceInfo)this._secondaryOutput.SelectedItem).Device;

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

                if (type == StreamType.Duplex && inputInfo.Id != outputInfo.Id)
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

                XtSystem system = (XtSystem)this._system.SelectedItem;
                OnXRun onXRunWrapper = new OnXRun(AddMessage);
                bool doLogXRuns = _logXRuns.CheckState == CheckState.Checked || (_logXRuns.CheckState == CheckState.Indeterminate && system != XtSystem.JACK);
                XtOnXRun onXRun = doLogXRuns ? onXRunWrapper.OnCallback : (XtOnXRun)null;

                XtFormat inputFormat = GetFormat(false).Value;
                inputFormat.channels.inputs = (int)_channelCount.SelectedItem;
                if (input && _inputChannels.SelectedItems.Count > 0 && _inputChannels.SelectedItems.Count != inputFormat.channels.inputs)
                {
                    MessageBox.Show(this,
                        "Selected either 0 input channels or a number equal to the selected format's channels.",
                        "Invalid input channel mask.");
                    return;
                }
                for (int c = 0; c < _inputChannels.SelectedItems.Count; c++)
                    inputFormat.channels.inMask |= (1UL << ((ChannelInfo)_inputChannels.SelectedItems[c]).Index);

                XtFormat outputFormat = GetFormat(true).Value;
                if (output && _outputChannels.SelectedItems.Count > 0 && _outputChannels.SelectedItems.Count != outputFormat.channels.outputs)
                {
                    MessageBox.Show(this,
                        "Selected either 0 output channels or a number equal to the selected format's channels.",
                        "Invalid output channel mask.");
                    return;
                }
                for (int c = 0; c < _outputChannels.SelectedItems.Count; c++)
                    outputFormat.channels.outMask |= (1UL << ((ChannelInfo)_outputChannels.SelectedItems[c]).Index);

                if (type == StreamType.Capture)
                {
                    _captureFile = new FileStream("xt-audio.raw", FileMode.Create, FileAccess.Write);
                    CaptureCallback callback = new CaptureCallback(_streamInterleaved.Checked, _streamNative.Checked, AddMessage, _captureFile);
                    var streamParams = new XtStreamParams(_streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    var deviceParams = new XtDeviceStreamParams(in streamParams, in inputFormat, _bufferSize.Value);
                    _inputStream = inputDevice.OpenStream(in deviceParams, "capture-user-data");
                    callback.Init(_inputStream.GetFormat(), _inputStream.GetFrames());
                    _safeBuffer = XtSafeBuffer.Register(_inputStream, _streamInterleaved.Checked);
                    _inputStream.Start();
                } else if (type == StreamType.Render)
                {
                    RenderCallback callback = new RenderCallback(_streamInterleaved.Checked, _streamNative.Checked, AddMessage);
                    var streamParams = new XtStreamParams(_streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    var deviceParams = new XtDeviceStreamParams(in streamParams, in outputFormat, _bufferSize.Value);
                    _outputStream = outputDevice.OpenStream(in deviceParams, "render-user-data");
                    _safeBuffer = XtSafeBuffer.Register(_outputStream, _streamInterleaved.Checked);
                    _outputStream.Start();
                } else if (type == StreamType.Duplex)
                {
                    XtFormat duplexFormat = inputFormat;
                    duplexFormat.channels.outputs = outputFormat.channels.outputs;
                    duplexFormat.channels.outMask = outputFormat.channels.outMask;
                    FullDuplexCallback callback = new FullDuplexCallback(_streamInterleaved.Checked, _streamNative.Checked, AddMessage);
                    var streamParams = new XtStreamParams(_streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    var deviceParams = new XtDeviceStreamParams(in streamParams, in duplexFormat, _bufferSize.Value);
                    _outputStream = outputDevice.OpenStream(in deviceParams, "duplex-user-data");
                    _safeBuffer = XtSafeBuffer.Register(_outputStream, _streamInterleaved.Checked);
                    _outputStream.Start();
                } else if (type == StreamType.Aggregate)
                {
                    var devices = new List<XtAggregateDeviceParams>();
                    if (inputDevice != null) devices.Add(new XtAggregateDeviceParams(inputDevice, new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0), _bufferSize.Value));
                    if (outputDevice != null) devices.Add(new XtAggregateDeviceParams(outputDevice, new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask), _bufferSize.Value));
                    if (secondaryInputDevice != null) devices.Add(new XtAggregateDeviceParams(secondaryInputDevice, new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0), _bufferSize.Value));
                    if (secondaryOutputDevice != null) devices.Add(new XtAggregateDeviceParams(secondaryOutputDevice, new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask), _bufferSize.Value));

                    XtDevice master = _outputMaster.Checked ?
                        (outputDevice != null ? outputDevice :
                        secondaryOutputDevice != null ? secondaryOutputDevice :
                        inputDevice != null ? inputDevice : secondaryInputDevice) :
                        (inputDevice != null ? inputDevice :
                        secondaryInputDevice != null ? secondaryInputDevice :
                        outputDevice != null ? outputDevice : secondaryOutputDevice);

                    AggregateCallback streamCallback = new AggregateCallback(_streamInterleaved.Checked, _streamNative.Checked, AddMessage);
                    var streamParams = new XtStreamParams(_streamInterleaved.Checked, streamCallback.OnCallback, onXRun, OnRunning);
                    var aggregateParams = new XtAggregateStreamParams(in streamParams, devices.ToArray(), devices.Count, outputFormat.mix, master);
                    _outputStream = _platform.GetService(((XtSystem)this._system.SelectedItem)).AggregateStream(in aggregateParams, "aggregate-user-data");
                    streamCallback.Init(_outputStream.GetFrames());
                    _safeBuffer = XtSafeBuffer.Register(_outputStream, _streamInterleaved.Checked);
                    _outputStream.Start();
                } else if (inputDevice == outputDevice)
                {
                    XtFormat duplexFormat = inputFormat;
                    duplexFormat.channels.outputs = outputFormat.channels.outputs;
                    duplexFormat.channels.outMask = outputFormat.channels.outMask;
                    LatencyCallback callback = new LatencyCallback(_streamInterleaved.Checked, _streamNative.Checked, AddMessage);
                    var streamParams = new XtStreamParams(_streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    var deviceParams = new XtDeviceStreamParams(in streamParams, in duplexFormat, _bufferSize.Value);
                    _outputStream = outputDevice.OpenStream(in deviceParams, "latency-user-data");
                    _safeBuffer = XtSafeBuffer.Register(_outputStream, _streamInterleaved.Checked);
                    _outputStream.Start();
                } else
                {
                    XtDevice master = _outputMaster.Checked ? outputDevice : inputDevice;
                    XtAggregateDeviceParams[] devices = new XtAggregateDeviceParams[2];
                    devices[0] = new XtAggregateDeviceParams(inputDevice, new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0), _bufferSize.Value);
                    devices[1] = new XtAggregateDeviceParams(outputDevice, new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask), _bufferSize.Value);
                    LatencyCallback callback = new LatencyCallback(_streamInterleaved.Checked, _streamNative.Checked, AddMessage);
                    XtStreamParams streamParams = new XtStreamParams(_streamInterleaved.Checked, callback.OnCallback, onXRun, OnRunning);
                    XtAggregateStreamParams aggregateParams = new XtAggregateStreamParams(in streamParams, devices, 2, in outputFormat.mix, master);
                    _outputStream = _platform.GetService(((XtSystem)this._system.SelectedItem)).AggregateStream(in aggregateParams, "latency-user-data");
                    _safeBuffer = XtSafeBuffer.Register(_outputStream, _streamInterleaved.Checked);
                    _outputStream.Start();
                }

            } catch (XtException e)
            {
                Stop();
                MessageBox.Show(this, XtAudio.GetErrorInfo(e.GetError()).ToString(), "Failed to start stream.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}