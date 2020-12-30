namespace Xt
{
    partial class DeviceView
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.Label label7;
            System.Windows.Forms.Label label15;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Label label13;
            System.Windows.Forms.Label label10;
            System.Windows.Forms.Label label12;
            System.Windows.Forms.Label label11;
            this._capabilities = new System.Windows.Forms.Label();
            this._interleaved = new System.Windows.Forms.Label();
            this._device = new System.Windows.Forms.ComboBox();
            this._channels = new System.Windows.Forms.ListBox();
            this._controlPanel = new System.Windows.Forms.Button();
            this._bufferSizes = new System.Windows.Forms.Label();
            this._mix = new System.Windows.Forms.Label();
            this._formatSupported = new System.Windows.Forms.Label();
            label7 = new System.Windows.Forms.Label();
            label15 = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            label13 = new System.Windows.Forms.Label();
            label10 = new System.Windows.Forms.Label();
            label12 = new System.Windows.Forms.Label();
            label11 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label7
            // 
            label7.AutoSize = true;
            label7.Location = new System.Drawing.Point(3, 87);
            label7.Name = "label7";
            label7.Size = new System.Drawing.Size(63, 13);
            label7.TabIndex = 82;
            label7.Text = "Capabilities:";
            // 
            // label15
            // 
            label15.AutoSize = true;
            label15.Location = new System.Drawing.Point(309, 60);
            label15.Name = "label15";
            label15.Size = new System.Drawing.Size(66, 13);
            label15.TabIndex = 80;
            label15.Text = "Interleaved: ";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            label2.Location = new System.Drawing.Point(3, 6);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(129, 13);
            label2.TabIndex = 69;
            label2.Text = "[DIRECTION] device:";
            // 
            // label13
            // 
            label13.AutoSize = true;
            label13.Location = new System.Drawing.Point(3, 35);
            label13.Name = "label13";
            label13.Size = new System.Drawing.Size(92, 13);
            label13.TabIndex = 74;
            label13.Text = "Format supported:";
            // 
            // label10
            // 
            label10.AutoSize = true;
            label10.Location = new System.Drawing.Point(3, 113);
            label10.Name = "label10";
            label10.Size = new System.Drawing.Size(77, 13);
            label10.TabIndex = 71;
            label10.Text = "Channel mask:";
            // 
            // label12
            // 
            label12.AutoSize = true;
            label12.Location = new System.Drawing.Point(174, 35);
            label12.Name = "label12";
            label12.Size = new System.Drawing.Size(62, 13);
            label12.TabIndex = 73;
            label12.Text = "Current mix:";
            // 
            // label11
            // 
            label11.AutoSize = true;
            label11.Location = new System.Drawing.Point(3, 60);
            label11.Name = "label11";
            label11.Size = new System.Drawing.Size(86, 13);
            label11.TabIndex = 72;
            label11.Text = "Buffer sizes (ms):";
            // 
            // _capabilities
            // 
            this._capabilities.AutoSize = true;
            this._capabilities.Location = new System.Drawing.Point(101, 87);
            this._capabilities.Name = "_capabilities";
            this._capabilities.Size = new System.Drawing.Size(33, 13);
            this._capabilities.TabIndex = 83;
            this._capabilities.Text = "None";
            // 
            // _interleaved
            // 
            this._interleaved.AutoSize = true;
            this._interleaved.Location = new System.Drawing.Point(391, 60);
            this._interleaved.Name = "_interleaved";
            this._interleaved.Size = new System.Drawing.Size(29, 13);
            this._interleaved.TabIndex = 81;
            this._interleaved.Text = "Both";
            // 
            // _device
            // 
            this._device.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._device.FormattingEnabled = true;
            this._device.Location = new System.Drawing.Point(104, 3);
            this._device.Name = "_device";
            this._device.Size = new System.Drawing.Size(316, 21);
            this._device.TabIndex = 70;
            // 
            // _channels
            // 
            this._channels.FormattingEnabled = true;
            this._channels.Location = new System.Drawing.Point(104, 113);
            this._channels.Name = "_channels";
            this._channels.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this._channels.Size = new System.Drawing.Size(316, 69);
            this._channels.TabIndex = 79;
            // 
            // _controlPanel
            // 
            this._controlPanel.Location = new System.Drawing.Point(312, 30);
            this._controlPanel.Name = "_controlPanel";
            this._controlPanel.Size = new System.Drawing.Size(108, 23);
            this._controlPanel.TabIndex = 78;
            this._controlPanel.Text = "Show control panel";
            this._controlPanel.UseVisualStyleBackColor = true;
            this._controlPanel.Click += new System.EventHandler(this.OnControlPanelClick);
            // 
            // _bufferSizes
            // 
            this._bufferSizes.AutoSize = true;
            this._bufferSizes.Location = new System.Drawing.Point(174, 60);
            this._bufferSizes.Name = "_bufferSizes";
            this._bufferSizes.Size = new System.Drawing.Size(98, 13);
            this._bufferSizes.TabIndex = 77;
            this._bufferSizes.Text = "3.0 / 20.0 / 2000.0";
            // 
            // _mix
            // 
            this._mix.AutoSize = true;
            this._mix.Location = new System.Drawing.Point(240, 35);
            this._mix.Name = "_mix";
            this._mix.Size = new System.Drawing.Size(64, 13);
            this._mix.TabIndex = 76;
            this._mix.Text = "44100 Int16";
            // 
            // _formatSupported
            // 
            this._formatSupported.AutoSize = true;
            this._formatSupported.Location = new System.Drawing.Point(101, 35);
            this._formatSupported.Name = "_formatSupported";
            this._formatSupported.Size = new System.Drawing.Size(32, 13);
            this._formatSupported.TabIndex = 75;
            this._formatSupported.Text = "False";
            // 
            // DeviceView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this._capabilities);
            this.Controls.Add(label7);
            this.Controls.Add(this._interleaved);
            this.Controls.Add(label15);
            this.Controls.Add(label2);
            this.Controls.Add(this._device);
            this.Controls.Add(this._channels);
            this.Controls.Add(this._controlPanel);
            this.Controls.Add(this._bufferSizes);
            this.Controls.Add(this._mix);
            this.Controls.Add(this._formatSupported);
            this.Controls.Add(label13);
            this.Controls.Add(label10);
            this.Controls.Add(label12);
            this.Controls.Add(label11);
            this.Name = "DeviceView";
            this.Size = new System.Drawing.Size(423, 187);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label _capabilities;
        private System.Windows.Forms.Label _interleaved;
        private System.Windows.Forms.Button _controlPanel;
        private System.Windows.Forms.Label _bufferSizes;
        private System.Windows.Forms.Label _mix;
        private System.Windows.Forms.Label _formatSupported;
        public System.Windows.Forms.ComboBox _device;
        public System.Windows.Forms.ListBox _channels;
    }
}
