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
            return result;
        }

        TextWriter _log;
        XtStream _stream;
        XtPlatform _platform;
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
            _stream?.Stop();
            _stream?.Dispose();
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

        void OnStart(object sender, EventArgs ea)
        {
            try
            {
                Start();
            } catch (XtException e)
            {
                Stop();
                var caption = "Failed to start stream.";
                var message = XtAudio.GetErrorInfo(e.GetError()).ToString();
                MessageBox.Show(this, message, caption, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
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

        object Start()
        {
            StreamType type = (StreamType)_streamType.SelectedItem;
            bool input = type == StreamType.Capture || type == StreamType.Duplex;
            bool output = type == StreamType.Render || type == StreamType.Duplex;
            var inputInfo = _input._device.SelectedItem as DeviceInfo;
            var outputInfo = _output._device.SelectedItem as DeviceInfo;
            XtDevice inputDevice = inputInfo?.Device;
            XtDevice outputDevice = outputInfo?.Device;
            XtDevice secondaryInputDevice = (_secondaryInput.SelectedItem as DeviceInfo).Device;
            XtDevice secondaryOutputDevice = (_secondaryOutput.SelectedItem as DeviceInfo).Device;

            bool anyInput = inputDevice != null || secondaryInputDevice != null;
            bool anyOutput = outputDevice != null || secondaryOutputDevice != null;
            string duplexMessage = "For duplex operation, input and output device must be the same.";
            string aggregateMessage = "For aggregate operation, select at least 1 input and 1 output device.";
            if (input && inputDevice == null) return MessageBox.Show(this, "Select an input device.", "Invalid input device.");
            if (output && outputDevice == null) return MessageBox.Show(this, "Select an output device.", "Invalid output device.");
            if (type == StreamType.Duplex && inputInfo.Id != outputInfo.Id) return MessageBox.Show(this, duplexMessage, "Invalid duplex device.");
            if (type == StreamType.Aggregate && (!anyInput || !anyOutput)) return MessageBox.Show(this, aggregateMessage, "Invalid aggregate device.");

            var state = _logXRuns.CheckState;
            XtSystem system = (XtSystem)_system.SelectedItem;
            OnXRun onXRunWrapper = new OnXRun(AddMessage);
            bool doLogXRuns = state == CheckState.Checked || (state == CheckState.Indeterminate && system != XtSystem.JACK);
            XtOnXRun onXRun = doLogXRuns ? onXRunWrapper.Callback : (XtOnXRun)null;

            XtFormat inputFormat = GetFormat(false).Value;
            var inputSelection = _input._channels.SelectedItems;
            inputFormat.channels.inputs = (int)_channelCount.SelectedItem;
            string inputChannelMessage = "Selected either 0 input channels or a number equal to the selected format's channels.";
            if (input && inputSelection.Count > 0 && inputSelection.Count != inputFormat.channels.inputs)
                return MessageBox.Show(this, inputChannelMessage, "Invalid input channel mask.");
            for (int c = 0; c < inputSelection.Count; c++)
                inputFormat.channels.inMask |= (1UL << ((ChannelInfo)inputSelection[c]).Index);

            XtFormat outputFormat = GetFormat(true).Value;
            var outputSelection = _output._channels.SelectedItems;
            outputFormat.channels.outputs = (int)_channelCount.SelectedItem;
            string outputChannelMessage = "Selected either 0 output channels or a number equal to the selected format's channels.";
            if (output && outputSelection.Count > 0 && outputSelection.Count != outputFormat.channels.outputs)
                return MessageBox.Show(this, outputChannelMessage, "Invalid output channel mask.");
            for (int c = 0; c < outputSelection.Count; c++)
                outputFormat.channels.outMask |= (1UL << ((ChannelInfo)outputSelection[c]).Index);

            int buffer = _bufferSize.Value;
            bool native = _streamNative.Checked;
            bool interleaved = _streamInterleaved.Checked;

            var @params = new OnBufferParams(interleaved, native, AddMessage);
            if (type == StreamType.Capture)
            {
                @params.Name = "Capture";
                _captureFile = new FileStream("xt-audio.raw", FileMode.Create, FileAccess.Write);
                OnCapture callback = new OnCapture(@params, _captureFile);
                var streamParams = new XtStreamParams(interleaved, callback.Callback, onXRun, OnRunning);
                var deviceParams = new XtDeviceStreamParams(in streamParams, in inputFormat, buffer);
                _stream = inputDevice.OpenStream(in deviceParams, "capture-user-data");
                callback.Init(_stream.GetFormat(), _stream.GetFrames());
                _safeBuffer = XtSafeBuffer.Register(_stream, interleaved);
                _stream.Start();
            } else if (type == StreamType.Render)
            {
                @params.Name = "Render";
                OnRender callback = new OnRender(@params);
                var streamParams = new XtStreamParams(interleaved, callback.Callback, onXRun, OnRunning);
                var deviceParams = new XtDeviceStreamParams(in streamParams, in outputFormat, buffer);
                _stream = outputDevice.OpenStream(in deviceParams, "render-user-data");
                _safeBuffer = XtSafeBuffer.Register(_stream, interleaved);
                _stream.Start();
            } else if (type == StreamType.Duplex)
            {
                @params.Name = "Duplex";
                XtFormat duplexFormat = inputFormat;
                duplexFormat.channels.outputs = outputFormat.channels.outputs;
                duplexFormat.channels.outMask = outputFormat.channels.outMask;
                OnFullDuplex callback = new OnFullDuplex(@params);
                var streamParams = new XtStreamParams(interleaved, callback.Callback, onXRun, OnRunning);
                var deviceParams = new XtDeviceStreamParams(in streamParams, in duplexFormat, buffer);
                _stream = outputDevice.OpenStream(in deviceParams, "duplex-user-data");
                _safeBuffer = XtSafeBuffer.Register(_stream, interleaved);
                _stream.Start();
            } else if (type == StreamType.Aggregate)
            {
                @params.Name = "Aggregate";
                var devices = new List<XtAggregateDeviceParams>();
                XtDevice master = _outputMaster.Checked ? (outputDevice ?? secondaryOutputDevice) : (inputDevice ?? secondaryInputDevice);
                if (inputDevice != null) devices.Add(new XtAggregateDeviceParams(inputDevice, inputFormat.channels, buffer));
                if (outputDevice != null) devices.Add(new XtAggregateDeviceParams(outputDevice, outputFormat.channels, buffer));
                if (secondaryInputDevice != null) devices.Add(new XtAggregateDeviceParams(secondaryInputDevice, inputFormat.channels, buffer));
                if (secondaryOutputDevice != null) devices.Add(new XtAggregateDeviceParams(secondaryOutputDevice, outputFormat.channels, buffer));
                OnAggregate streamCallback = new OnAggregate(@params);
                var streamParams = new XtStreamParams(interleaved, streamCallback.Callback, onXRun, OnRunning);
                var aggregateParams = new XtAggregateStreamParams(in streamParams, devices.ToArray(), devices.Count, outputFormat.mix, master);
                _stream = _platform.GetService(system).AggregateStream(in aggregateParams, "aggregate-user-data");
                streamCallback.Init(_stream.GetFrames());
                _safeBuffer = XtSafeBuffer.Register(_stream, interleaved);
                _stream.Start();
            }
            return null;
        }
    }
}