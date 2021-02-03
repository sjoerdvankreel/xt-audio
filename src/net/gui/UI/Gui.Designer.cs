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
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label label4;
            System.Windows.Forms.Label label6;
            System.Windows.Forms.Label label8;
            System.Windows.Forms.Label label9;
            System.Windows.Forms.Label label14;
            System.Windows.Forms.Label label21;
            this._guiLog = new System.Windows.Forms.TextBox();
            this._system = new System.Windows.Forms.ComboBox();
            this._serviceCaps = new System.Windows.Forms.Label();
            this._defaultInput = new System.Windows.Forms.Label();
            this._defaultOutput = new System.Windows.Forms.Label();
            this._rate = new System.Windows.Forms.ComboBox();
            this._channelCount = new System.Windows.Forms.ComboBox();
            this._sample = new System.Windows.Forms.ComboBox();
            this._attributes = new System.Windows.Forms.Label();
            this._start = new System.Windows.Forms.Button();
            this._stop = new System.Windows.Forms.Button();
            this.panel = new System.Windows.Forms.Panel();
            this._input = new Xt.DeviceView();
            this._bufferSize = new System.Windows.Forms.TrackBar();
            this._streamType = new System.Windows.Forms.ComboBox();
            this._streamInterleaved = new System.Windows.Forms.CheckBox();
            this._streamNative = new System.Windows.Forms.CheckBox();
            this._outputMaster = new System.Windows.Forms.CheckBox();
            this._secondaryInput = new System.Windows.Forms.ComboBox();
            this._secondaryOutput = new System.Windows.Forms.ComboBox();
            this._logXRuns = new System.Windows.Forms.CheckBox();
            this._output = new Xt.DeviceView();
            label1 = new System.Windows.Forms.Label();
            label3 = new System.Windows.Forms.Label();
            label4 = new System.Windows.Forms.Label();
            label6 = new System.Windows.Forms.Label();
            label8 = new System.Windows.Forms.Label();
            label9 = new System.Windows.Forms.Label();
            label14 = new System.Windows.Forms.Label();
            label21 = new System.Windows.Forms.Label();
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
            this.panel.Controls.Add(this._output);
            this.panel.Controls.Add(this._input);
            this.panel.Controls.Add(label1);
            this.panel.Controls.Add(this._system);
            this.panel.Controls.Add(this._attributes);
            this.panel.Controls.Add(label9);
            this.panel.Controls.Add(this._serviceCaps);
            this.panel.Controls.Add(this._defaultInput);
            this.panel.Controls.Add(this._defaultOutput);
            this.panel.Controls.Add(label3);
            this.panel.Controls.Add(label4);
            this.panel.Controls.Add(label6);
            this.panel.Controls.Add(this._rate);
            this.panel.Controls.Add(this._channelCount);
            this.panel.Controls.Add(this._sample);
            this.panel.Controls.Add(label8);
            this.panel.Location = new System.Drawing.Point(12, 12);
            this.panel.Name = "panel";
            this.panel.Size = new System.Drawing.Size(429, 548);
            this.panel.TabIndex = 64;
            // 
            // _input
            // 
            this._input.Location = new System.Drawing.Point(0, 166);
            this._input.Name = "_input";
            this._input.Size = new System.Drawing.Size(423, 187);
            this._input.TabIndex = 61;
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
            // _output
            // 
            this._output.Location = new System.Drawing.Point(0, 350);
            this._output.Name = "_output";
            this._output.Size = new System.Drawing.Size(423, 187);
            this._output.TabIndex = 62;
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
        private System.Windows.Forms.Label _serviceCaps;
        private System.Windows.Forms.Label _defaultInput;
        private System.Windows.Forms.Label _defaultOutput;
        private System.Windows.Forms.ComboBox _rate;
        private System.Windows.Forms.ComboBox _channelCount;
        private System.Windows.Forms.ComboBox _sample;
        private System.Windows.Forms.Label _attributes;
        private System.Windows.Forms.Button _start;
        private System.Windows.Forms.Button _stop;
        private System.Windows.Forms.Panel panel;
        private System.Windows.Forms.TrackBar _bufferSize;
        private System.Windows.Forms.ComboBox _streamType;
        private System.Windows.Forms.CheckBox _streamInterleaved;
        private System.Windows.Forms.CheckBox _streamNative;
        private System.Windows.Forms.CheckBox _outputMaster;
        private System.Windows.Forms.ComboBox _secondaryInput;
        private System.Windows.Forms.ComboBox _secondaryOutput;
        private System.Windows.Forms.CheckBox _logXRuns;
        private DeviceView _input;
        private DeviceView _output;
    }
}