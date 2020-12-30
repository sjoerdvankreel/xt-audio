namespace Xt
{
    partial class XtGui
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.Label label1;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label label4;
            System.Windows.Forms.Label label6;
            System.Windows.Forms.Label label5;
            System.Windows.Forms.Label label8;
            System.Windows.Forms.Label label10;
            System.Windows.Forms.Label label11;
            System.Windows.Forms.Label label12;
            System.Windows.Forms.Label label13;
            System.Windows.Forms.Label label16;
            System.Windows.Forms.Label label17;
            System.Windows.Forms.Label label18;
            System.Windows.Forms.Label label19;
            System.Windows.Forms.Label label9;
            System.Windows.Forms.Label label14;
            System.Windows.Forms.Label label15;
            System.Windows.Forms.Label label20;
            System.Windows.Forms.Label label21;
            System.Windows.Forms.Label label7;
            System.Windows.Forms.Label label22;
            this._guiLog = new System.Windows.Forms.TextBox();
            this._system = new System.Windows.Forms.ComboBox();
            this._inputDevice = new System.Windows.Forms.ComboBox();
            this._serviceCaps = new System.Windows.Forms.Label();
            this._defaultInput = new System.Windows.Forms.Label();
            this._defaultOutput = new System.Windows.Forms.Label();
            this._outputDevice = new System.Windows.Forms.ComboBox();
            this._rate = new System.Windows.Forms.ComboBox();
            this._channelCount = new System.Windows.Forms.ComboBox();
            this._sample = new System.Windows.Forms.ComboBox();
            this._inputFormatSupported = new System.Windows.Forms.Label();
            this._inputMix = new System.Windows.Forms.Label();
            this._inputBufferSizes = new System.Windows.Forms.Label();
            this._inputControlPanel = new System.Windows.Forms.Button();
            this._outputControlPanel = new System.Windows.Forms.Button();
            this._outputBufferSizes = new System.Windows.Forms.Label();
            this._outputMix = new System.Windows.Forms.Label();
            this._outputFormatSupported = new System.Windows.Forms.Label();
            this._inputChannels = new System.Windows.Forms.ListBox();
            this._outputChannels = new System.Windows.Forms.ListBox();
            this._attributes = new System.Windows.Forms.Label();
            this._start = new System.Windows.Forms.Button();
            this._stop = new System.Windows.Forms.Button();
            this.panel = new System.Windows.Forms.Panel();
            this._outputCaps = new System.Windows.Forms.Label();
            this._inputCaps = new System.Windows.Forms.Label();
            this._outputInterleaved = new System.Windows.Forms.Label();
            this._inputInterleaved = new System.Windows.Forms.Label();
            this._bufferSize = new System.Windows.Forms.TrackBar();
            this._streamType = new System.Windows.Forms.ComboBox();
            this._streamInterleaved = new System.Windows.Forms.CheckBox();
            this._streamNative = new System.Windows.Forms.CheckBox();
            this._outputMaster = new System.Windows.Forms.CheckBox();
            this._secondaryInput = new System.Windows.Forms.ComboBox();
            this._secondaryOutput = new System.Windows.Forms.ComboBox();
            this._logXRuns = new System.Windows.Forms.CheckBox();
            label1 = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            label3 = new System.Windows.Forms.Label();
            label4 = new System.Windows.Forms.Label();
            label6 = new System.Windows.Forms.Label();
            label5 = new System.Windows.Forms.Label();
            label8 = new System.Windows.Forms.Label();
            label10 = new System.Windows.Forms.Label();
            label11 = new System.Windows.Forms.Label();
            label12 = new System.Windows.Forms.Label();
            label13 = new System.Windows.Forms.Label();
            label16 = new System.Windows.Forms.Label();
            label17 = new System.Windows.Forms.Label();
            label18 = new System.Windows.Forms.Label();
            label19 = new System.Windows.Forms.Label();
            label9 = new System.Windows.Forms.Label();
            label14 = new System.Windows.Forms.Label();
            label15 = new System.Windows.Forms.Label();
            label20 = new System.Windows.Forms.Label();
            label21 = new System.Windows.Forms.Label();
            label7 = new System.Windows.Forms.Label();
            label22 = new System.Windows.Forms.Label();
            this.panel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this._bufferSize)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            label1.Location = new System.Drawing.Point(2, 6);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(51, 13);
            label1.TabIndex = 3;
            label1.Text = "System:";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            label2.Location = new System.Drawing.Point(3, 169);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(82, 13);
            label2.TabIndex = 5;
            label2.Text = "Input device:";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new System.Drawing.Point(3, 61);
            label3.Name = "label3";
            label3.Size = new System.Drawing.Size(73, 13);
            label3.TabIndex = 12;
            label3.Text = "Default input: ";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new System.Drawing.Point(3, 87);
            label4.Name = "label4";
            label4.Size = new System.Drawing.Size(77, 13);
            label4.TabIndex = 13;
            label4.Text = "Default output:";
            // 
            // label6
            // 
            label6.AutoSize = true;
            label6.Location = new System.Drawing.Point(3, 34);
            label6.Name = "label6";
            label6.Size = new System.Drawing.Size(63, 13);
            label6.TabIndex = 15;
            label6.Text = "Capabilities:";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            label5.Location = new System.Drawing.Point(2, 361);
            label5.Name = "label5";
            label5.Size = new System.Drawing.Size(91, 13);
            label5.TabIndex = 21;
            label5.Text = "Output device:";
            // 
            // label8
            // 
            label8.AutoSize = true;
            label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            label8.Location = new System.Drawing.Point(3, 115);
            label8.Name = "label8";
            label8.Size = new System.Drawing.Size(49, 13);
            label8.TabIndex = 27;
            label8.Text = "Format:";
            // 
            // label10
            // 
            label10.AutoSize = true;
            label10.Location = new System.Drawing.Point(3, 276);
            label10.Name = "label10";
            label10.Size = new System.Drawing.Size(77, 13);
            label10.TabIndex = 39;
            label10.Text = "Channel mask:";
            // 
            // label11
            // 
            label11.AutoSize = true;
            label11.Location = new System.Drawing.Point(3, 223);
            label11.Name = "label11";
            label11.Size = new System.Drawing.Size(86, 13);
            label11.TabIndex = 40;
            label11.Text = "Buffer sizes (ms):";
            // 
            // label12
            // 
            label12.AutoSize = true;
            label12.Location = new System.Drawing.Point(136, 196);
            label12.Name = "label12";
            label12.Size = new System.Drawing.Size(62, 13);
            label12.TabIndex = 41;
            label12.Text = "Current mix:";
            // 
            // label13
            // 
            label13.AutoSize = true;
            label13.Location = new System.Drawing.Point(3, 196);
            label13.Name = "label13";
            label13.Size = new System.Drawing.Size(92, 13);
            label13.TabIndex = 42;
            label13.Text = "Format supported:";
            // 
            // label16
            // 
            label16.AutoSize = true;
            label16.Location = new System.Drawing.Point(2, 388);
            label16.Name = "label16";
            label16.Size = new System.Drawing.Size(92, 13);
            label16.TabIndex = 51;
            label16.Text = "Format supported:";
            // 
            // label17
            // 
            label17.AutoSize = true;
            label17.Location = new System.Drawing.Point(135, 388);
            label17.Name = "label17";
            label17.Size = new System.Drawing.Size(62, 13);
            label17.TabIndex = 50;
            label17.Text = "Current mix:";
            // 
            // label18
            // 
            label18.AutoSize = true;
            label18.Location = new System.Drawing.Point(2, 415);
            label18.Name = "label18";
            label18.Size = new System.Drawing.Size(86, 13);
            label18.TabIndex = 49;
            label18.Text = "Buffer sizes (ms):";
            // 
            // label19
            // 
            label19.AutoSize = true;
            label19.Location = new System.Drawing.Point(3, 468);
            label19.Name = "label19";
            label19.Size = new System.Drawing.Size(77, 13);
            label19.TabIndex = 48;
            label19.Text = "Channel mask:";
            // 
            // label9
            // 
            label9.AutoSize = true;
            label9.Location = new System.Drawing.Point(3, 142);
            label9.Name = "label9";
            label9.Size = new System.Drawing.Size(91, 13);
            label9.TabIndex = 59;
            label9.Text = "Sample attributes:";
            // 
            // label14
            // 
            label14.AutoSize = true;
            label14.Location = new System.Drawing.Point(447, 485);
            label14.Name = "label14";
            label14.Size = new System.Drawing.Size(81, 13);
            label14.TabIndex = 66;
            label14.Text = "Buffer size (ms):";
            // 
            // label15
            // 
            label15.AutoSize = true;
            label15.Location = new System.Drawing.Point(306, 223);
            label15.Name = "label15";
            label15.Size = new System.Drawing.Size(66, 13);
            label15.TabIndex = 63;
            label15.Text = "Interleaved: ";
            // 
            // label20
            // 
            label20.AutoSize = true;
            label20.Location = new System.Drawing.Point(305, 415);
            label20.Name = "label20";
            label20.Size = new System.Drawing.Size(66, 13);
            label20.TabIndex = 64;
            label20.Text = "Interleaved: ";
            // 
            // label21
            // 
            label21.AutoSize = true;
            label21.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            label21.Location = new System.Drawing.Point(447, 374);
            label21.Name = "label21";
            label21.Size = new System.Drawing.Size(305, 13);
            label21.TabIndex = 75;
            label21.Text = "Stream aggregation secondary input/output devices:";
            // 
            // label7
            // 
            label7.AutoSize = true;
            label7.Location = new System.Drawing.Point(3, 250);
            label7.Name = "label7";
            label7.Size = new System.Drawing.Size(63, 13);
            label7.TabIndex = 67;
            label7.Text = "Capabilities:";
            // 
            // label22
            // 
            label22.AutoSize = true;
            label22.Location = new System.Drawing.Point(3, 442);
            label22.Name = "label22";
            label22.Size = new System.Drawing.Size(63, 13);
            label22.TabIndex = 69;
            label22.Text = "Capabilities:";
            // 
            // _guiLog
            // 
            this._guiLog.Location = new System.Drawing.Point(447, 12);
            this._guiLog.Multiline = true;
            this._guiLog.Name = "_guiLog";
            this._guiLog.ReadOnly = true;
            this._guiLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this._guiLog.Size = new System.Drawing.Size(325, 353);
            this._guiLog.TabIndex = 0;
            // 
            // _system
            // 
            this._system.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._system.FormattingEnabled = true;
            this._system.Location = new System.Drawing.Point(100, 3);
            this._system.Name = "_system";
            this._system.Size = new System.Drawing.Size(316, 21);
            this._system.TabIndex = 4;
            this._system.SelectedIndexChanged += new System.EventHandler(this.OnSystemChanged);
            // 
            // _inputDevice
            // 
            this._inputDevice.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._inputDevice.FormattingEnabled = true;
            this._inputDevice.Location = new System.Drawing.Point(101, 166);
            this._inputDevice.Name = "_inputDevice";
            this._inputDevice.Size = new System.Drawing.Size(316, 21);
            this._inputDevice.TabIndex = 6;
            this._inputDevice.SelectedIndexChanged += new System.EventHandler(this.OnDeviceChanged);
            // 
            // _serviceCaps
            // 
            this._serviceCaps.AutoSize = true;
            this._serviceCaps.Location = new System.Drawing.Point(98, 34);
            this._serviceCaps.Name = "_serviceCaps";
            this._serviceCaps.Size = new System.Drawing.Size(33, 13);
            this._serviceCaps.TabIndex = 7;
            this._serviceCaps.Text = "None";
            // 
            // _defaultInput
            // 
            this._defaultInput.AutoSize = true;
            this._defaultInput.Location = new System.Drawing.Point(98, 61);
            this._defaultInput.Name = "_defaultInput";
            this._defaultInput.Size = new System.Drawing.Size(198, 13);
            this._defaultInput.TabIndex = 9;
            this._defaultInput.Text = "Realtek Digital Input (Realtek HD Audio)";
            // 
            // _defaultOutput
            // 
            this._defaultOutput.AutoSize = true;
            this._defaultOutput.Location = new System.Drawing.Point(98, 87);
            this._defaultOutput.Name = "_defaultOutput";
            this._defaultOutput.Size = new System.Drawing.Size(206, 13);
            this._defaultOutput.TabIndex = 10;
            this._defaultOutput.Text = "Realtek Digital Output (Realtek HD Audio)";
            // 
            // _outputDevice
            // 
            this._outputDevice.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._outputDevice.FormattingEnabled = true;
            this._outputDevice.Location = new System.Drawing.Point(100, 358);
            this._outputDevice.Name = "_outputDevice";
            this._outputDevice.Size = new System.Drawing.Size(316, 21);
            this._outputDevice.TabIndex = 22;
            this._outputDevice.SelectedIndexChanged += new System.EventHandler(this.OnDeviceChanged);
            // 
            // _rate
            // 
            this._rate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._rate.FormattingEnabled = true;
            this._rate.Location = new System.Drawing.Point(101, 112);
            this._rate.Name = "_rate";
            this._rate.Size = new System.Drawing.Size(103, 21);
            this._rate.TabIndex = 24;
            this._rate.SelectedIndexChanged += new System.EventHandler(this.OnFormatChanged);
            // 
            // _channelCount
            // 
            this._channelCount.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._channelCount.FormattingEnabled = true;
            this._channelCount.Location = new System.Drawing.Point(319, 112);
            this._channelCount.Name = "_channelCount";
            this._channelCount.Size = new System.Drawing.Size(98, 21);
            this._channelCount.TabIndex = 25;
            this._channelCount.SelectedIndexChanged += new System.EventHandler(this.OnFormatChanged);
            // 
            // _sample
            // 
            this._sample.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._sample.FormattingEnabled = true;
            this._sample.Location = new System.Drawing.Point(210, 112);
            this._sample.Name = "_sample";
            this._sample.Size = new System.Drawing.Size(103, 21);
            this._sample.TabIndex = 26;
            this._sample.SelectedIndexChanged += new System.EventHandler(this.OnFormatChanged);
            // 
            // _inputFormatSupported
            // 
            this._inputFormatSupported.AutoSize = true;
            this._inputFormatSupported.Location = new System.Drawing.Point(98, 196);
            this._inputFormatSupported.Name = "_inputFormatSupported";
            this._inputFormatSupported.Size = new System.Drawing.Size(32, 13);
            this._inputFormatSupported.TabIndex = 43;
            this._inputFormatSupported.Text = "False";
            // 
            // _inputMix
            // 
            this._inputMix.AutoSize = true;
            this._inputMix.Location = new System.Drawing.Point(204, 196);
            this._inputMix.Name = "_inputMix";
            this._inputMix.Size = new System.Drawing.Size(64, 13);
            this._inputMix.TabIndex = 44;
            this._inputMix.Text = "44100 Int16";
            // 
            // _inputBufferSizes
            // 
            this._inputBufferSizes.AutoSize = true;
            this._inputBufferSizes.Location = new System.Drawing.Point(98, 223);
            this._inputBufferSizes.Name = "_inputBufferSizes";
            this._inputBufferSizes.Size = new System.Drawing.Size(98, 13);
            this._inputBufferSizes.TabIndex = 45;
            this._inputBufferSizes.Text = "3.0 / 20.0 / 2000.0";
            // 
            // _inputControlPanel
            // 
            this._inputControlPanel.Location = new System.Drawing.Point(309, 193);
            this._inputControlPanel.Name = "_inputControlPanel";
            this._inputControlPanel.Size = new System.Drawing.Size(108, 23);
            this._inputControlPanel.TabIndex = 46;
            this._inputControlPanel.Text = "Show control panel";
            this._inputControlPanel.UseVisualStyleBackColor = true;
            this._inputControlPanel.Click += new System.EventHandler(this.OnShowInputPanel);
            // 
            // _outputControlPanel
            // 
            this._outputControlPanel.Location = new System.Drawing.Point(308, 385);
            this._outputControlPanel.Name = "_outputControlPanel";
            this._outputControlPanel.Size = new System.Drawing.Size(108, 23);
            this._outputControlPanel.TabIndex = 55;
            this._outputControlPanel.Text = "Show control panel";
            this._outputControlPanel.UseVisualStyleBackColor = true;
            this._outputControlPanel.Click += new System.EventHandler(this.OnShowOutputPanel);
            // 
            // _outputBufferSizes
            // 
            this._outputBufferSizes.AutoSize = true;
            this._outputBufferSizes.Location = new System.Drawing.Point(97, 415);
            this._outputBufferSizes.Name = "_outputBufferSizes";
            this._outputBufferSizes.Size = new System.Drawing.Size(98, 13);
            this._outputBufferSizes.TabIndex = 54;
            this._outputBufferSizes.Text = "3.0 / 20.0 / 2000.0";
            // 
            // _outputMix
            // 
            this._outputMix.AutoSize = true;
            this._outputMix.Location = new System.Drawing.Point(203, 388);
            this._outputMix.Name = "_outputMix";
            this._outputMix.Size = new System.Drawing.Size(64, 13);
            this._outputMix.TabIndex = 53;
            this._outputMix.Text = "44100 Int16";
            // 
            // _outputFormatSupported
            // 
            this._outputFormatSupported.AutoSize = true;
            this._outputFormatSupported.Location = new System.Drawing.Point(97, 388);
            this._outputFormatSupported.Name = "_outputFormatSupported";
            this._outputFormatSupported.Size = new System.Drawing.Size(32, 13);
            this._outputFormatSupported.TabIndex = 52;
            this._outputFormatSupported.Text = "False";
            // 
            // _inputChannels
            // 
            this._inputChannels.FormattingEnabled = true;
            this._inputChannels.Location = new System.Drawing.Point(101, 276);
            this._inputChannels.Name = "_inputChannels";
            this._inputChannels.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this._inputChannels.Size = new System.Drawing.Size(316, 69);
            this._inputChannels.TabIndex = 56;
            // 
            // _outputChannels
            // 
            this._outputChannels.FormattingEnabled = true;
            this._outputChannels.Location = new System.Drawing.Point(100, 468);
            this._outputChannels.Name = "_outputChannels";
            this._outputChannels.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this._outputChannels.Size = new System.Drawing.Size(316, 69);
            this._outputChannels.TabIndex = 57;
            // 
            // _attributes
            // 
            this._attributes.AutoSize = true;
            this._attributes.Location = new System.Drawing.Point(98, 142);
            this._attributes.Name = "_attributes";
            this._attributes.Size = new System.Drawing.Size(149, 13);
            this._attributes.TabIndex = 60;
            this._attributes.Text = "[size: 2, isFloat: 0, isSigned: 0]";
            // 
            // _start
            // 
            this._start.Location = new System.Drawing.Point(616, 526);
            this._start.Name = "_start";
            this._start.Size = new System.Drawing.Size(75, 23);
            this._start.TabIndex = 62;
            this._start.Text = "Start";
            this._start.UseVisualStyleBackColor = true;
            this._start.Click += new System.EventHandler(this.OnStart);
            // 
            // _stop
            // 
            this._stop.Location = new System.Drawing.Point(697, 526);
            this._stop.Name = "_stop";
            this._stop.Size = new System.Drawing.Size(75, 23);
            this._stop.TabIndex = 63;
            this._stop.Text = "Stop";
            this._stop.UseVisualStyleBackColor = true;
            this._stop.Click += new System.EventHandler(this.OnStop);
            // 
            // panel
            // 
            this.panel.Controls.Add(this._outputCaps);
            this.panel.Controls.Add(label22);
            this.panel.Controls.Add(this._inputCaps);
            this.panel.Controls.Add(label7);
            this.panel.Controls.Add(this._outputInterleaved);
            this.panel.Controls.Add(this._inputInterleaved);
            this.panel.Controls.Add(label20);
            this.panel.Controls.Add(label15);
            this.panel.Controls.Add(label1);
            this.panel.Controls.Add(this._system);
            this.panel.Controls.Add(this._attributes);
            this.panel.Controls.Add(label2);
            this.panel.Controls.Add(label9);
            this.panel.Controls.Add(this._inputDevice);
            this.panel.Controls.Add(this._outputChannels);
            this.panel.Controls.Add(this._serviceCaps);
            this.panel.Controls.Add(this._inputChannels);
            this.panel.Controls.Add(this._defaultInput);
            this.panel.Controls.Add(this._outputControlPanel);
            this.panel.Controls.Add(this._defaultOutput);
            this.panel.Controls.Add(this._outputBufferSizes);
            this.panel.Controls.Add(this._outputMix);
            this.panel.Controls.Add(label3);
            this.panel.Controls.Add(this._outputFormatSupported);
            this.panel.Controls.Add(label4);
            this.panel.Controls.Add(label16);
            this.panel.Controls.Add(label6);
            this.panel.Controls.Add(label17);
            this.panel.Controls.Add(label18);
            this.panel.Controls.Add(label5);
            this.panel.Controls.Add(label19);
            this.panel.Controls.Add(this._outputDevice);
            this.panel.Controls.Add(this._inputControlPanel);
            this.panel.Controls.Add(this._rate);
            this.panel.Controls.Add(this._inputBufferSizes);
            this.panel.Controls.Add(this._channelCount);
            this.panel.Controls.Add(this._inputMix);
            this.panel.Controls.Add(this._sample);
            this.panel.Controls.Add(this._inputFormatSupported);
            this.panel.Controls.Add(label8);
            this.panel.Controls.Add(label13);
            this.panel.Controls.Add(label10);
            this.panel.Controls.Add(label12);
            this.panel.Controls.Add(label11);
            this.panel.Location = new System.Drawing.Point(12, 12);
            this.panel.Name = "panel";
            this.panel.Size = new System.Drawing.Size(429, 548);
            this.panel.TabIndex = 64;
            // 
            // _outputCaps
            // 
            this._outputCaps.AutoSize = true;
            this._outputCaps.Location = new System.Drawing.Point(97, 442);
            this._outputCaps.Name = "_outputCaps";
            this._outputCaps.Size = new System.Drawing.Size(33, 13);
            this._outputCaps.TabIndex = 70;
            this._outputCaps.Text = "None";
            // 
            // _inputCaps
            // 
            this._inputCaps.AutoSize = true;
            this._inputCaps.Location = new System.Drawing.Point(98, 250);
            this._inputCaps.Name = "_inputCaps";
            this._inputCaps.Size = new System.Drawing.Size(33, 13);
            this._inputCaps.TabIndex = 68;
            this._inputCaps.Text = "None";
            // 
            // _outputInterleaved
            // 
            this._outputInterleaved.AutoSize = true;
            this._outputInterleaved.Location = new System.Drawing.Point(377, 415);
            this._outputInterleaved.Name = "_outputInterleaved";
            this._outputInterleaved.Size = new System.Drawing.Size(29, 13);
            this._outputInterleaved.TabIndex = 66;
            this._outputInterleaved.Text = "Both";
            // 
            // _inputInterleaved
            // 
            this._inputInterleaved.AutoSize = true;
            this._inputInterleaved.Location = new System.Drawing.Point(378, 223);
            this._inputInterleaved.Name = "_inputInterleaved";
            this._inputInterleaved.Size = new System.Drawing.Size(29, 13);
            this._inputInterleaved.TabIndex = 65;
            this._inputInterleaved.Text = "Both";
            // 
            // _bufferSize
            // 
            this._bufferSize.Location = new System.Drawing.Point(534, 475);
            this._bufferSize.Name = "_bufferSize";
            this._bufferSize.Size = new System.Drawing.Size(238, 45);
            this._bufferSize.TabIndex = 65;
            this._bufferSize.Scroll += new System.EventHandler(this.OnBufferSizeScroll);
            // 
            // _streamType
            // 
            this._streamType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._streamType.FormattingEnabled = true;
            this._streamType.Location = new System.Drawing.Point(450, 528);
            this._streamType.Name = "_streamType";
            this._streamType.Size = new System.Drawing.Size(121, 21);
            this._streamType.TabIndex = 67;
            // 
            // _streamInterleaved
            // 
            this._streamInterleaved.AutoSize = true;
            this._streamInterleaved.Checked = true;
            this._streamInterleaved.CheckState = System.Windows.Forms.CheckState.Checked;
            this._streamInterleaved.Location = new System.Drawing.Point(450, 452);
            this._streamInterleaved.Name = "_streamInterleaved";
            this._streamInterleaved.Size = new System.Drawing.Size(79, 17);
            this._streamInterleaved.TabIndex = 68;
            this._streamInterleaved.Text = "Interleaved";
            this._streamInterleaved.UseVisualStyleBackColor = true;
            // 
            // _streamNative
            // 
            this._streamNative.AutoSize = true;
            this._streamNative.Location = new System.Drawing.Point(535, 452);
            this._streamNative.Name = "_streamNative";
            this._streamNative.Size = new System.Drawing.Size(57, 17);
            this._streamNative.TabIndex = 69;
            this._streamNative.Text = "Native";
            this._streamNative.UseVisualStyleBackColor = true;
            // 
            // _outputMaster
            // 
            this._outputMaster.AutoSize = true;
            this._outputMaster.Checked = true;
            this._outputMaster.CheckState = System.Windows.Forms.CheckState.Checked;
            this._outputMaster.Location = new System.Drawing.Point(598, 452);
            this._outputMaster.Name = "_outputMaster";
            this._outputMaster.Size = new System.Drawing.Size(92, 17);
            this._outputMaster.TabIndex = 70;
            this._outputMaster.Text = "Output master";
            this._outputMaster.UseVisualStyleBackColor = true;
            // 
            // _secondaryInput
            // 
            this._secondaryInput.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._secondaryInput.FormattingEnabled = true;
            this._secondaryInput.Location = new System.Drawing.Point(447, 398);
            this._secondaryInput.Name = "_secondaryInput";
            this._secondaryInput.Size = new System.Drawing.Size(325, 21);
            this._secondaryInput.TabIndex = 73;
            // 
            // _secondaryOutput
            // 
            this._secondaryOutput.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._secondaryOutput.FormattingEnabled = true;
            this._secondaryOutput.Location = new System.Drawing.Point(447, 425);
            this._secondaryOutput.Name = "_secondaryOutput";
            this._secondaryOutput.Size = new System.Drawing.Size(325, 21);
            this._secondaryOutput.TabIndex = 74;
            // 
            // _logXRuns
            // 
            this._logXRuns.AutoSize = true;
            this._logXRuns.Checked = true;
            this._logXRuns.CheckState = System.Windows.Forms.CheckState.Indeterminate;
            this._logXRuns.Location = new System.Drawing.Point(696, 452);
            this._logXRuns.Name = "_logXRuns";
            this._logXRuns.Size = new System.Drawing.Size(72, 17);
            this._logXRuns.TabIndex = 76;
            this._logXRuns.Text = "Log xruns";
            this._logXRuns.ThreeState = true;
            this._logXRuns.UseVisualStyleBackColor = true;
            // 
            // XtGui
            // 
            this.ClientSize = new System.Drawing.Size(784, 561);
            this.Controls.Add(this._logXRuns);
            this.Controls.Add(label21);
            this.Controls.Add(this._secondaryOutput);
            this.Controls.Add(this._secondaryInput);
            this.Controls.Add(this._outputMaster);
            this.Controls.Add(this._streamNative);
            this.Controls.Add(this._streamInterleaved);
            this.Controls.Add(this._streamType);
            this.Controls.Add(label14);
            this.Controls.Add(this._bufferSize);
            this.Controls.Add(this.panel);
            this.Controls.Add(this._stop);
            this.Controls.Add(this._start);
            this.Controls.Add(this._guiLog);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(800, 600);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(800, 600);
            this.Name = "XtGui";
            this.Text = "XT-Audio";
            this.panel.ResumeLayout(false);
            this.panel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this._bufferSize)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox _guiLog;
        private System.Windows.Forms.ComboBox _system;
        private System.Windows.Forms.ComboBox _inputDevice;
        private System.Windows.Forms.Label _serviceCaps;
        private System.Windows.Forms.Label _defaultInput;
        private System.Windows.Forms.Label _defaultOutput;
        private System.Windows.Forms.ComboBox _outputDevice;
        private System.Windows.Forms.ComboBox _rate;
        private System.Windows.Forms.ComboBox _channelCount;
        private System.Windows.Forms.ComboBox _sample;
        private System.Windows.Forms.Label _inputFormatSupported;
        private System.Windows.Forms.Label _inputMix;
        private System.Windows.Forms.Label _inputBufferSizes;
        private System.Windows.Forms.Button _inputControlPanel;
        private System.Windows.Forms.Button _outputControlPanel;
        private System.Windows.Forms.Label _outputBufferSizes;
        private System.Windows.Forms.Label _outputMix;
        private System.Windows.Forms.Label _outputFormatSupported;
        private System.Windows.Forms.ListBox _inputChannels;
        private System.Windows.Forms.ListBox _outputChannels;
        private System.Windows.Forms.Label _attributes;
        private System.Windows.Forms.Button _start;
        private System.Windows.Forms.Button _stop;
        private System.Windows.Forms.Panel panel;
        private System.Windows.Forms.TrackBar _bufferSize;
        private System.Windows.Forms.ComboBox _streamType;
        private System.Windows.Forms.Label _outputInterleaved;
        private System.Windows.Forms.Label _inputInterleaved;
        private System.Windows.Forms.CheckBox _streamInterleaved;
        private System.Windows.Forms.CheckBox _streamNative;
        private System.Windows.Forms.CheckBox _outputMaster;
        private System.Windows.Forms.ComboBox _secondaryInput;
        private System.Windows.Forms.ComboBox _secondaryOutput;
        private System.Windows.Forms.CheckBox _logXRuns;
        private System.Windows.Forms.Label _inputCaps;
        private System.Windows.Forms.Label _outputCaps;
    }
}