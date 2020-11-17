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
			this.messages = new System.Windows.Forms.TextBox();
			this.system = new System.Windows.Forms.ComboBox();
			this.inputDevice = new System.Windows.Forms.ComboBox();
			this.capabilities = new System.Windows.Forms.Label();
			this.defaultInput = new System.Windows.Forms.Label();
			this.defaultOutput = new System.Windows.Forms.Label();
			this.outputDevice = new System.Windows.Forms.ComboBox();
			this.rate = new System.Windows.Forms.ComboBox();
			this.channelCount = new System.Windows.Forms.ComboBox();
			this.sample = new System.Windows.Forms.ComboBox();
			this.inputFormatSupported = new System.Windows.Forms.Label();
			this.inputMix = new System.Windows.Forms.Label();
			this.inputBufferSizes = new System.Windows.Forms.Label();
			this.inputControlPanel = new System.Windows.Forms.Button();
			this.outputControlPanel = new System.Windows.Forms.Button();
			this.outputBufferSizes = new System.Windows.Forms.Label();
			this.outputMix = new System.Windows.Forms.Label();
			this.outputFormatSupported = new System.Windows.Forms.Label();
			this.inputChannels = new System.Windows.Forms.ListBox();
			this.outputChannels = new System.Windows.Forms.ListBox();
			this.attributes = new System.Windows.Forms.Label();
			this.start = new System.Windows.Forms.Button();
			this.stop = new System.Windows.Forms.Button();
			this.panel = new System.Windows.Forms.Panel();
			this.outputInterleaved = new System.Windows.Forms.Label();
			this.inputInterleaved = new System.Windows.Forms.Label();
			this.bufferSize = new System.Windows.Forms.TrackBar();
			this.streamType = new System.Windows.Forms.ComboBox();
			this.streamInterleaved = new System.Windows.Forms.CheckBox();
			this.streamRaw = new System.Windows.Forms.CheckBox();
			this.outputMaster = new System.Windows.Forms.CheckBox();
			this.secondaryInput = new System.Windows.Forms.ComboBox();
			this.secondaryOutput = new System.Windows.Forms.ComboBox();
			this.logXRuns = new System.Windows.Forms.CheckBox();
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
			this.panel.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.bufferSize)).BeginInit();
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
			label10.Location = new System.Drawing.Point(3, 250);
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
			label19.Location = new System.Drawing.Point(2, 442);
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
			// messages
			// 
			this.messages.Location = new System.Drawing.Point(447, 12);
			this.messages.Multiline = true;
			this.messages.Name = "messages";
			this.messages.ReadOnly = true;
			this.messages.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.messages.Size = new System.Drawing.Size(325, 353);
			this.messages.TabIndex = 0;
			// 
			// system
			// 
			this.system.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.system.FormattingEnabled = true;
			this.system.Location = new System.Drawing.Point(100, 3);
			this.system.Name = "system";
			this.system.Size = new System.Drawing.Size(316, 21);
			this.system.TabIndex = 4;
			this.system.SelectedIndexChanged += new System.EventHandler(this.OnSystemChanged);
			// 
			// inputDevice
			// 
			this.inputDevice.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.inputDevice.FormattingEnabled = true;
			this.inputDevice.Location = new System.Drawing.Point(101, 166);
			this.inputDevice.Name = "inputDevice";
			this.inputDevice.Size = new System.Drawing.Size(316, 21);
			this.inputDevice.TabIndex = 6;
			this.inputDevice.SelectedIndexChanged += new System.EventHandler(this.OnDeviceChanged);
			// 
			// capabilities
			// 
			this.capabilities.AutoSize = true;
			this.capabilities.Location = new System.Drawing.Point(98, 34);
			this.capabilities.Name = "capabilities";
			this.capabilities.Size = new System.Drawing.Size(33, 13);
			this.capabilities.TabIndex = 7;
			this.capabilities.Text = "None";
			// 
			// defaultInput
			// 
			this.defaultInput.AutoSize = true;
			this.defaultInput.Location = new System.Drawing.Point(98, 61);
			this.defaultInput.Name = "defaultInput";
			this.defaultInput.Size = new System.Drawing.Size(198, 13);
			this.defaultInput.TabIndex = 9;
			this.defaultInput.Text = "Realtek Digital Input (Realtek HD Audio)";
			// 
			// defaultOutput
			// 
			this.defaultOutput.AutoSize = true;
			this.defaultOutput.Location = new System.Drawing.Point(98, 87);
			this.defaultOutput.Name = "defaultOutput";
			this.defaultOutput.Size = new System.Drawing.Size(206, 13);
			this.defaultOutput.TabIndex = 10;
			this.defaultOutput.Text = "Realtek Digital Output (Realtek HD Audio)";
			// 
			// outputDevice
			// 
			this.outputDevice.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.outputDevice.FormattingEnabled = true;
			this.outputDevice.Location = new System.Drawing.Point(100, 358);
			this.outputDevice.Name = "outputDevice";
			this.outputDevice.Size = new System.Drawing.Size(316, 21);
			this.outputDevice.TabIndex = 22;
			this.outputDevice.SelectedIndexChanged += new System.EventHandler(this.OnDeviceChanged);
			// 
			// rate
			// 
			this.rate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.rate.FormattingEnabled = true;
			this.rate.Location = new System.Drawing.Point(101, 112);
			this.rate.Name = "rate";
			this.rate.Size = new System.Drawing.Size(103, 21);
			this.rate.TabIndex = 24;
			this.rate.SelectedIndexChanged += new System.EventHandler(this.OnFormatChanged);
			// 
			// channelCount
			// 
			this.channelCount.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.channelCount.FormattingEnabled = true;
			this.channelCount.Location = new System.Drawing.Point(319, 112);
			this.channelCount.Name = "channelCount";
			this.channelCount.Size = new System.Drawing.Size(98, 21);
			this.channelCount.TabIndex = 25;
			this.channelCount.SelectedIndexChanged += new System.EventHandler(this.OnFormatChanged);
			// 
			// sample
			// 
			this.sample.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.sample.FormattingEnabled = true;
			this.sample.Location = new System.Drawing.Point(210, 112);
			this.sample.Name = "sample";
			this.sample.Size = new System.Drawing.Size(103, 21);
			this.sample.TabIndex = 26;
			this.sample.SelectedIndexChanged += new System.EventHandler(this.OnFormatChanged);
			// 
			// inputFormatSupported
			// 
			this.inputFormatSupported.AutoSize = true;
			this.inputFormatSupported.Location = new System.Drawing.Point(98, 196);
			this.inputFormatSupported.Name = "inputFormatSupported";
			this.inputFormatSupported.Size = new System.Drawing.Size(32, 13);
			this.inputFormatSupported.TabIndex = 43;
			this.inputFormatSupported.Text = "False";
			// 
			// inputMix
			// 
			this.inputMix.AutoSize = true;
			this.inputMix.Location = new System.Drawing.Point(204, 196);
			this.inputMix.Name = "inputMix";
			this.inputMix.Size = new System.Drawing.Size(64, 13);
			this.inputMix.TabIndex = 44;
			this.inputMix.Text = "44100 Int16";
			// 
			// inputBufferSizes
			// 
			this.inputBufferSizes.AutoSize = true;
			this.inputBufferSizes.Location = new System.Drawing.Point(98, 223);
			this.inputBufferSizes.Name = "inputBufferSizes";
			this.inputBufferSizes.Size = new System.Drawing.Size(98, 13);
			this.inputBufferSizes.TabIndex = 45;
			this.inputBufferSizes.Text = "3.0 / 20.0 / 2000.0";
			// 
			// inputControlPanel
			// 
			this.inputControlPanel.Location = new System.Drawing.Point(309, 193);
			this.inputControlPanel.Name = "inputControlPanel";
			this.inputControlPanel.Size = new System.Drawing.Size(108, 23);
			this.inputControlPanel.TabIndex = 46;
			this.inputControlPanel.Text = "Show control panel";
			this.inputControlPanel.UseVisualStyleBackColor = true;
			this.inputControlPanel.Click += new System.EventHandler(this.OnShowInputPanel);
			// 
			// outputControlPanel
			// 
			this.outputControlPanel.Location = new System.Drawing.Point(308, 385);
			this.outputControlPanel.Name = "outputControlPanel";
			this.outputControlPanel.Size = new System.Drawing.Size(108, 23);
			this.outputControlPanel.TabIndex = 55;
			this.outputControlPanel.Text = "Show control panel";
			this.outputControlPanel.UseVisualStyleBackColor = true;
			this.outputControlPanel.Click += new System.EventHandler(this.OnShowOutputPanel);
			// 
			// outputBufferSizes
			// 
			this.outputBufferSizes.AutoSize = true;
			this.outputBufferSizes.Location = new System.Drawing.Point(97, 415);
			this.outputBufferSizes.Name = "outputBufferSizes";
			this.outputBufferSizes.Size = new System.Drawing.Size(98, 13);
			this.outputBufferSizes.TabIndex = 54;
			this.outputBufferSizes.Text = "3.0 / 20.0 / 2000.0";
			// 
			// outputMix
			// 
			this.outputMix.AutoSize = true;
			this.outputMix.Location = new System.Drawing.Point(203, 388);
			this.outputMix.Name = "outputMix";
			this.outputMix.Size = new System.Drawing.Size(64, 13);
			this.outputMix.TabIndex = 53;
			this.outputMix.Text = "44100 Int16";
			// 
			// outputFormatSupported
			// 
			this.outputFormatSupported.AutoSize = true;
			this.outputFormatSupported.Location = new System.Drawing.Point(97, 388);
			this.outputFormatSupported.Name = "outputFormatSupported";
			this.outputFormatSupported.Size = new System.Drawing.Size(32, 13);
			this.outputFormatSupported.TabIndex = 52;
			this.outputFormatSupported.Text = "False";
			// 
			// inputChannels
			// 
			this.inputChannels.FormattingEnabled = true;
			this.inputChannels.Location = new System.Drawing.Point(101, 250);
			this.inputChannels.Name = "inputChannels";
			this.inputChannels.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.inputChannels.Size = new System.Drawing.Size(316, 95);
			this.inputChannels.TabIndex = 56;
			// 
			// outputChannels
			// 
			this.outputChannels.FormattingEnabled = true;
			this.outputChannels.Location = new System.Drawing.Point(100, 442);
			this.outputChannels.Name = "outputChannels";
			this.outputChannels.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.outputChannels.Size = new System.Drawing.Size(316, 95);
			this.outputChannels.TabIndex = 57;
			// 
			// attributes
			// 
			this.attributes.AutoSize = true;
			this.attributes.Location = new System.Drawing.Point(98, 142);
			this.attributes.Name = "attributes";
			this.attributes.Size = new System.Drawing.Size(149, 13);
			this.attributes.TabIndex = 60;
			this.attributes.Text = "[size: 2, isFloat: 0, isSigned: 0]";
			// 
			// start
			// 
			this.start.Location = new System.Drawing.Point(616, 526);
			this.start.Name = "start";
			this.start.Size = new System.Drawing.Size(75, 23);
			this.start.TabIndex = 62;
			this.start.Text = "Start";
			this.start.UseVisualStyleBackColor = true;
			this.start.Click += new System.EventHandler(this.OnStart);
			// 
			// stop
			// 
			this.stop.Location = new System.Drawing.Point(697, 526);
			this.stop.Name = "stop";
			this.stop.Size = new System.Drawing.Size(75, 23);
			this.stop.TabIndex = 63;
			this.stop.Text = "Stop";
			this.stop.UseVisualStyleBackColor = true;
			this.stop.Click += new System.EventHandler(this.OnStop);
			// 
			// panel
			// 
			this.panel.Controls.Add(this.outputInterleaved);
			this.panel.Controls.Add(this.inputInterleaved);
			this.panel.Controls.Add(label20);
			this.panel.Controls.Add(label15);
			this.panel.Controls.Add(label1);
			this.panel.Controls.Add(this.system);
			this.panel.Controls.Add(this.attributes);
			this.panel.Controls.Add(label2);
			this.panel.Controls.Add(label9);
			this.panel.Controls.Add(this.inputDevice);
			this.panel.Controls.Add(this.outputChannels);
			this.panel.Controls.Add(this.capabilities);
			this.panel.Controls.Add(this.inputChannels);
			this.panel.Controls.Add(this.defaultInput);
			this.panel.Controls.Add(this.outputControlPanel);
			this.panel.Controls.Add(this.defaultOutput);
			this.panel.Controls.Add(this.outputBufferSizes);
			this.panel.Controls.Add(this.outputMix);
			this.panel.Controls.Add(label3);
			this.panel.Controls.Add(this.outputFormatSupported);
			this.panel.Controls.Add(label4);
			this.panel.Controls.Add(label16);
			this.panel.Controls.Add(label6);
			this.panel.Controls.Add(label17);
			this.panel.Controls.Add(label18);
			this.panel.Controls.Add(label5);
			this.panel.Controls.Add(label19);
			this.panel.Controls.Add(this.outputDevice);
			this.panel.Controls.Add(this.inputControlPanel);
			this.panel.Controls.Add(this.rate);
			this.panel.Controls.Add(this.inputBufferSizes);
			this.panel.Controls.Add(this.channelCount);
			this.panel.Controls.Add(this.inputMix);
			this.panel.Controls.Add(this.sample);
			this.panel.Controls.Add(this.inputFormatSupported);
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
			// outputInterleaved
			// 
			this.outputInterleaved.AutoSize = true;
			this.outputInterleaved.Location = new System.Drawing.Point(377, 415);
			this.outputInterleaved.Name = "outputInterleaved";
			this.outputInterleaved.Size = new System.Drawing.Size(29, 13);
			this.outputInterleaved.TabIndex = 66;
			this.outputInterleaved.Text = "Both";
			// 
			// inputInterleaved
			// 
			this.inputInterleaved.AutoSize = true;
			this.inputInterleaved.Location = new System.Drawing.Point(378, 223);
			this.inputInterleaved.Name = "inputInterleaved";
			this.inputInterleaved.Size = new System.Drawing.Size(29, 13);
			this.inputInterleaved.TabIndex = 65;
			this.inputInterleaved.Text = "Both";
			// 
			// bufferSize
			// 
			this.bufferSize.Location = new System.Drawing.Point(534, 475);
			this.bufferSize.Name = "bufferSize";
			this.bufferSize.Size = new System.Drawing.Size(238, 45);
			this.bufferSize.TabIndex = 65;
			this.bufferSize.Scroll += new System.EventHandler(this.OnBufferSizeScroll);
			// 
			// streamType
			// 
			this.streamType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.streamType.FormattingEnabled = true;
			this.streamType.Location = new System.Drawing.Point(450, 528);
			this.streamType.Name = "streamType";
			this.streamType.Size = new System.Drawing.Size(121, 21);
			this.streamType.TabIndex = 67;
			// 
			// streamInterleaved
			// 
			this.streamInterleaved.AutoSize = true;
			this.streamInterleaved.Checked = true;
			this.streamInterleaved.CheckState = System.Windows.Forms.CheckState.Checked;
			this.streamInterleaved.Location = new System.Drawing.Point(450, 452);
			this.streamInterleaved.Name = "streamInterleaved";
			this.streamInterleaved.Size = new System.Drawing.Size(79, 17);
			this.streamInterleaved.TabIndex = 68;
			this.streamInterleaved.Text = "Interleaved";
			this.streamInterleaved.UseVisualStyleBackColor = true;
			// 
			// streamRaw
			// 
			this.streamRaw.AutoSize = true;
			this.streamRaw.Location = new System.Drawing.Point(535, 452);
			this.streamRaw.Name = "streamRaw";
			this.streamRaw.Size = new System.Drawing.Size(48, 17);
			this.streamRaw.TabIndex = 69;
			this.streamRaw.Text = "Raw";
			this.streamRaw.UseVisualStyleBackColor = true;
			// 
			// outputMaster
			// 
			this.outputMaster.AutoSize = true;
			this.outputMaster.Checked = true;
			this.outputMaster.CheckState = System.Windows.Forms.CheckState.Checked;
			this.outputMaster.Location = new System.Drawing.Point(589, 452);
			this.outputMaster.Name = "outputMaster";
			this.outputMaster.Size = new System.Drawing.Size(92, 17);
			this.outputMaster.TabIndex = 70;
			this.outputMaster.Text = "Output master";
			this.outputMaster.UseVisualStyleBackColor = true;
			// 
			// secondaryInput
			// 
			this.secondaryInput.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.secondaryInput.FormattingEnabled = true;
			this.secondaryInput.Location = new System.Drawing.Point(447, 398);
			this.secondaryInput.Name = "secondaryInput";
			this.secondaryInput.Size = new System.Drawing.Size(325, 21);
			this.secondaryInput.TabIndex = 73;
			// 
			// secondaryOutput
			// 
			this.secondaryOutput.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.secondaryOutput.FormattingEnabled = true;
			this.secondaryOutput.Location = new System.Drawing.Point(447, 425);
			this.secondaryOutput.Name = "secondaryOutput";
			this.secondaryOutput.Size = new System.Drawing.Size(325, 21);
			this.secondaryOutput.TabIndex = 74;
			// 
			// logXRuns
			// 
			this.logXRuns.AutoSize = true;
			this.logXRuns.Checked = true;
			this.logXRuns.CheckState = System.Windows.Forms.CheckState.Indeterminate;
			this.logXRuns.Location = new System.Drawing.Point(687, 452);
			this.logXRuns.Name = "logXRuns";
			this.logXRuns.Size = new System.Drawing.Size(72, 17);
			this.logXRuns.TabIndex = 76;
			this.logXRuns.Text = "Log xruns";
			this.logXRuns.ThreeState = true;
			this.logXRuns.UseVisualStyleBackColor = true;
			// 
			// XtGui
			// 
			this.ClientSize = new System.Drawing.Size(784, 561);
			this.Controls.Add(this.logXRuns);
			this.Controls.Add(label21);
			this.Controls.Add(this.secondaryOutput);
			this.Controls.Add(this.secondaryInput);
			this.Controls.Add(this.outputMaster);
			this.Controls.Add(this.streamRaw);
			this.Controls.Add(this.streamInterleaved);
			this.Controls.Add(this.streamType);
			this.Controls.Add(label14);
			this.Controls.Add(this.bufferSize);
			this.Controls.Add(this.panel);
			this.Controls.Add(this.stop);
			this.Controls.Add(this.start);
			this.Controls.Add(this.messages);
			this.MaximizeBox = false;
			this.MaximumSize = new System.Drawing.Size(800, 600);
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(800, 600);
			this.Name = "XtGui";
			this.Text = "XT-Audio";
			this.panel.ResumeLayout(false);
			this.panel.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.bufferSize)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox messages;
        private System.Windows.Forms.ComboBox system;
        private System.Windows.Forms.ComboBox inputDevice;
        private System.Windows.Forms.Label capabilities;
        private System.Windows.Forms.Label defaultInput;
        private System.Windows.Forms.Label defaultOutput;
        private System.Windows.Forms.ComboBox outputDevice;
        private System.Windows.Forms.ComboBox rate;
        private System.Windows.Forms.ComboBox channelCount;
        private System.Windows.Forms.ComboBox sample;
        private System.Windows.Forms.Label inputFormatSupported;
        private System.Windows.Forms.Label inputMix;
        private System.Windows.Forms.Label inputBufferSizes;
        private System.Windows.Forms.Button inputControlPanel;
        private System.Windows.Forms.Button outputControlPanel;
        private System.Windows.Forms.Label outputBufferSizes;
        private System.Windows.Forms.Label outputMix;
        private System.Windows.Forms.Label outputFormatSupported;
        private System.Windows.Forms.ListBox inputChannels;
        private System.Windows.Forms.ListBox outputChannels;
        private System.Windows.Forms.Label attributes;
        private System.Windows.Forms.Button start;
        private System.Windows.Forms.Button stop;
        private System.Windows.Forms.Panel panel;
        private System.Windows.Forms.TrackBar bufferSize;
        private System.Windows.Forms.ComboBox streamType;
        private System.Windows.Forms.Label outputInterleaved;
        private System.Windows.Forms.Label inputInterleaved;
        private System.Windows.Forms.CheckBox streamInterleaved;
        private System.Windows.Forms.CheckBox streamRaw;
        private System.Windows.Forms.CheckBox outputMaster;
        private System.Windows.Forms.ComboBox secondaryInput;
        private System.Windows.Forms.ComboBox secondaryOutput;
        private System.Windows.Forms.CheckBox logXRuns;
    }
}