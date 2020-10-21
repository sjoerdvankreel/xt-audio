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

		private static readonly List<StreamType> StreamTypes
			= Enum.GetValues(typeof(StreamType)).Cast<StreamType>().ToList();

		[STAThread]
		public static void Main(string[] args)
		{
			Application.EnableVisualStyles();
			Application.Run(new XtGui());
		}

		private XtAudio audio;
		private TextWriter log;
		private XtStream inputStream;
		private XtStream outputStream;
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

		private void OnServiceChanged(object sender, EventArgs e)
		{
			ServiceChanged();
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

		private void OnTrace(XtLevel level, string message)
		{
			AddMessage(() => string.Format("{0}: {1}", level, message), level != XtLevel.Info);
		}

		private void OnStreamError(Func<string> error)
		{
			AddMessage(error, true);
			BeginInvoke(new Action(() => Stop()));
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

		private void OnFatal()
		{
			string trace = Environment.StackTrace;
			string message = "A fatal error occurred." + Environment.NewLine + "StackTrace:" + Environment.NewLine + trace;
			Invoke(new Action(() =>
			{
				MessageBox.Show(this, message, "Fatal error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
				Environment.FailFast("Fatal error.");
			}));
		}

		protected override void OnClosing(CancelEventArgs e)
		{
			base.OnClosing(e);
			Stop();
			ClearDevices();
			audio.Dispose();
			log.Dispose();
		}

		private void AddMessage(Func<string> message)
		{
			AddMessage(message, false);
		}

		private void AddMessage(Func<string> message, bool error)
		{
			messages.BeginInvoke(new Action(() =>
			{
				string msg = message();
				log.WriteLine(msg);
				if (error)
					log.Flush();
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
			log = new StreamWriter("xt-audio.log");

			audio = new XtAudio("XT-Gui", Handle, OnTrace, OnFatal);
			debug.Text = "Debug: False";
#if DEBUG
			debug.Text = "Debug: True";
#endif
			x64.Text = "X64: " + (IntPtr.Size == 8);
			isWin32.Text = "Win32: " + XtAudio.IsWin32();
			version.Text = "Version: " + XtAudio.GetVersionMajor() + "." + XtAudio.GetVersionMinor();

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

		private void ServiceChanged()
		{
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

			for (int i = 0; i < s.GetDeviceCount(); i++)
			{
				DeviceView view = new DeviceView();
				view.device = s.OpenDevice(i);
				view.index = i;
				deviceViews.Add(view);
			}

			List<DeviceView> inputViews = (from v in deviceViews
										   where v.defaultInput || v.device.GetChannelCount(false) > 0
										   select v).ToList();
			inputViews.Insert(0, new DeviceView());

			List<DeviceView> outputViews = (from v in deviceViews
											where v.defaultOutput || v.device.GetChannelCount(true) > 0
											select v).ToList();
			outputViews.Insert(0, new DeviceView());

			inputDevice.DataSource = new List<DeviceView>(inputViews);
			outputDevice.DataSource = new List<DeviceView>(outputViews);
			secondaryInput.DataSource = new List<DeviceView>(inputViews);
			secondaryOutput.DataSource = new List<DeviceView>(outputViews);
			inputDevice.SelectedIndex = inputViews.Count == 1 ? 0 : 1;
			outputDevice.SelectedIndex = outputViews.Count == 1 ? 0 : 1;

			system.Text = s.GetSystem().ToString();
			capabilities.Text = s.GetCapabilities().ToString();
			defaultInput.Text = inputView.device == null ? "null" : inputView.device.ToString();
			defaultOutput.Text = outputView.device == null ? "null" : outputView.device.ToString();
			inputControlPanel.Enabled = s.GetSystem() == XtSystem.Asio;
			outputControlPanel.Enabled = s.GetSystem() == XtSystem.Asio;
		}

		private void FormatOrDeviceChanged()
		{
			if (sample.SelectedItem != null)
			{
				var attrs = XtAudio.GetSampleAttributes((XtSample)sample.SelectedItem);
				attributes.Text = $"Size: {attrs.size}, Float: {attrs.isFloat}, Signed: {attrs.isSigned}";
			}

			XtFormat? inputFormat = GetFormat(false);
			XtDevice inputDevice = this.inputDevice.SelectedItem == null ?
				null : ((DeviceView)(this.inputDevice.SelectedItem)).device;
			bool inputSupported = inputDevice == null || inputFormat == null ? false : inputDevice.SupportsFormat(inputFormat.Value);
			inputFormatSupported.Text = inputSupported.ToString();
			XtBuffer? inputBuffer = !inputSupported ? (XtBuffer?)null : inputDevice.GetBuffer(inputFormat.Value);
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
			XtBuffer? outputBuffer = !outputSupported ? (XtBuffer?)null : outputDevice.GetBuffer(outputFormat.Value);
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

			stop.Enabled = false;
			panel.Enabled = true;
			start.Enabled = true;
			streamRaw.Enabled = true;
			bufferSize.Enabled = true;
			streamType.Enabled = true;
			outputMaster.Enabled = true;
			secondaryInput.Enabled = true;
			secondaryOutput.Enabled = true;
			streamInterleaved.Enabled = true;
		}

		private void OnStart(object sender, EventArgs ea)
		{
			try
			{
				StreamType type = (StreamType)streamType.SelectedItem;
				bool input = type == StreamType.Capture || type == StreamType.Duplex || type == StreamType.Latency;
				bool output = type == StreamType.Render || type == StreamType.Duplex || type == StreamType.Latency;
				XtDevice inputDevice = ((DeviceView)this.inputDevice.SelectedItem).device;
				XtDevice outputDevice = ((DeviceView)this.outputDevice.SelectedItem).device;
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

				if (type == StreamType.Duplex && outputDevice != inputDevice)
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

				XtSystem system = ((XtService)service.SelectedItem).GetSystem();
				XRunCallback xRunCallbackWrapper = new XRunCallback(AddMessage);
				bool doLogXRuns = logXRuns.CheckState == CheckState.Checked || (logXRuns.CheckState == CheckState.Indeterminate && system != XtSystem.Jack);
				XtXRunCallback xRunCallback = doLogXRuns ? xRunCallbackWrapper.OnCallback : (XtXRunCallback)null;

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

				stop.Enabled = true;
				panel.Enabled = false;
				start.Enabled = false;
				streamRaw.Enabled = false;
				bufferSize.Enabled = false;
				streamType.Enabled = false;
				outputMaster.Enabled = false;
				secondaryInput.Enabled = false;
				secondaryOutput.Enabled = false;
				streamInterleaved.Enabled = false;

				if (type == StreamType.Capture)
				{
					captureFile = new FileStream("xt-audio.raw", FileMode.Create, FileAccess.Write);
					CaptureCallback callback = new CaptureCallback(OnStreamError, AddMessage, captureFile);
					inputStream = inputDevice.OpenStream(inputFormat, streamInterleaved.Checked, streamRaw.Checked,
						bufferSize.Value, callback.OnCallback, xRunCallback, "capture-user-data");
					callback.Init(inputStream.GetFormat(), inputStream.GetFrames());
					inputStream.Start();

				}
				else if (type == StreamType.Render)
				{
					RenderCallback callback = new RenderCallback(OnStreamError, AddMessage);
					outputStream = outputDevice.OpenStream(outputFormat, streamInterleaved.Checked, streamRaw.Checked,
						bufferSize.Value, callback.OnCallback, xRunCallback, "render-user-data");
					outputStream.Start();

				}
				else if (type == StreamType.Duplex)
				{
					XtFormat duplexFormat = inputFormat;
					duplexFormat.channels.outputs = outputFormat.channels.outputs;
					duplexFormat.channels.outMask = outputFormat.channels.outMask;
					FullDuplexCallback callback = new FullDuplexCallback(OnStreamError, AddMessage);
					outputStream = outputDevice.OpenStream(duplexFormat, streamInterleaved.Checked, streamRaw.Checked,
						bufferSize.Value, callback.OnCallback, xRunCallback, "duplex-user-data");
					outputStream.Start();

				}
				else if (type == StreamType.Aggregate)
				{
					List<XtDevice> devices = new List<XtDevice>();
					List<double> bufferSizes = new List<double>();
					List<XtChannels> channels = new List<XtChannels>();
					if (inputDevice != null)
					{
						devices.Add(inputDevice);
						bufferSizes.Add(bufferSize.Value);
						channels.Add(new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0));
					}
					if (outputDevice != null)
					{
						devices.Add(outputDevice);
						bufferSizes.Add(bufferSize.Value);
						channels.Add(new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask));
					}
					if (secondaryInputDevice != null)
					{
						devices.Add(secondaryInputDevice);
						bufferSizes.Add(bufferSize.Value);
						channels.Add(new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0));
					}
					if (secondaryOutputDevice != null)
					{
						devices.Add(secondaryOutputDevice);
						bufferSizes.Add(bufferSize.Value);
						channels.Add(new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask));
					}

					XtDevice[] devicesArray = devices.ToArray();
					double[] bufferSizesArray = bufferSizes.ToArray();
					XtChannels[] channelsArray = channels.ToArray();
					XtDevice master = outputMaster.Checked ?
						(outputDevice != null ? outputDevice :
						secondaryOutputDevice != null ? secondaryOutputDevice :
						inputDevice != null ? inputDevice : secondaryInputDevice) :
						(inputDevice != null ? inputDevice :
						secondaryInputDevice != null ? secondaryInputDevice :
						outputDevice != null ? outputDevice : secondaryOutputDevice);

					AggregateCallback streamCallback = new AggregateCallback(OnStreamError, AddMessage);
					outputStream = ((XtService)service.SelectedItem).AggregateStream(devicesArray, channelsArray,
						bufferSizesArray, devicesArray.Length, outputFormat.mix, streamInterleaved.Checked, streamRaw.Checked,
						master, streamCallback.OnCallback, xRunCallback, "aggregate-user-data");
					streamCallback.Init(outputStream.GetFrames());
					outputStream.Start();

				}
				else if (inputDevice == outputDevice)
				{
					XtFormat duplexFormat = inputFormat;
					duplexFormat.channels.outputs = outputFormat.channels.outputs;
					duplexFormat.channels.outMask = outputFormat.channels.outMask;
					LatencyCallback callback = new LatencyCallback(OnStreamError, AddMessage);
					outputStream = outputDevice.OpenStream(duplexFormat, streamInterleaved.Checked, streamRaw.Checked,
						bufferSize.Value, callback.OnCallback, xRunCallback, "latency-user-data");
					outputStream.Start();

				}
				else
				{
					XtDevice[] devices = new XtDevice[] { inputDevice, outputDevice };
					double[] bufferSizes = new double[] { bufferSize.Value, bufferSize.Value };
					XtChannels[] channels = new XtChannels[] {
						new XtChannels(inputFormat.channels.inputs, inputFormat.channels.inMask, 0, 0),
						new XtChannels(0, 0, outputFormat.channels.outputs, outputFormat.channels.outMask)
					};
					XtDevice master = outputMaster.Checked ? outputDevice : inputDevice;
					LatencyCallback callback = new LatencyCallback(OnStreamError, AddMessage);
					outputStream = ((XtService)service.SelectedItem).AggregateStream(devices, channels,
						bufferSizes, devices.Length, outputFormat.mix, streamInterleaved.Checked, streamRaw.Checked,
						master, callback.OnCallback, xRunCallback, "latency-user-data");
					outputStream.Start();
				}

			}
			catch (XtException e)
			{
				Stop();
				MessageBox.Show(this, e.ToString(), "Failed to start stream.", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}
	}
}