using System;
using System.Runtime.InteropServices;

/* Copyright (C) 2015-2016 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */
namespace Xt {

    public sealed class XtStream : IDisposable {

        private IntPtr s;
        private Array input;
        private Array output;
        private readonly object user;
        private readonly XtStreamCallback callback;
        internal XtNative.StreamCallbackNet netCallback;
        internal XtNative.StreamCallbackMono monoCallback;

        internal XtStream(XtStreamCallback callback, object user) {
            this.user = user;
            this.callback = callback;
        }

        public XtSystem GetSystem() {
            return XtNative.XtStreamGetSystem(s);
        }

        public bool IsInterleaved() {
            return XtNative.XtStreamIsInterleaved(s);
        }

        public void Stop() {
            XtNative.HandleError(XtNative.XtStreamStop(s));
        }

        public void Start() {
            XtNative.HandleError(XtNative.XtStreamStart(s));
        }

        public XtFormat GetFormat() {
            return ((XtNative.Format)Marshal.PtrToStructure(XtNative.XtStreamGetFormat(s), typeof(XtNative.Format))).FromNative();
        }

        public void Dispose() {
            if (s != IntPtr.Zero)
                XtNative.XtStreamDestroy(s);
            s = IntPtr.Zero;
        }

        public int GetFrames() {
            int frames;
            XtNative.HandleError(XtNative.XtStreamGetFrames(s, out frames));
            return frames;
        }

        public XtLatency GetLatency() {
            XtLatency latency = new XtLatency();
            XtNative.HandleError(XtNative.XtStreamGetLatency(s, latency));
            return latency;
        }

        internal void Init(IntPtr s) {

            this.s = s;
            int frames = GetFrames();
            XtFormat format = GetFormat();
            switch (format.mix.sample) {
                case XtSample.UInt8:
                    input = new byte[format.inputs * frames];
                    output = new byte[format.outputs * frames];
                    break;
                case XtSample.Int16:
                    input = new short[format.inputs * frames];
                    output = new short[format.outputs * frames];
                    break;
                case XtSample.Int24:
                    input = new byte[format.inputs * 3 * frames];
                    output = new byte[format.outputs * 3 * frames];
                    break;
                case XtSample.Int32:
                    input = new int[format.inputs * frames];
                    output = new int[format.outputs * frames];
                    break;
                case XtSample.Float32:
                    input = new float[format.inputs * frames];
                    output = new float[format.outputs * frames];
                    break;
                default:
                    throw new ArgumentException();
            }
        }

        internal void Callback(IntPtr stream, IntPtr input, IntPtr output, int frames,
            double time, ulong position, bool timeValid, ulong error, IntPtr u) {

            XtFormat format = GetFormat();
            Array inData = input == IntPtr.Zero ? null : this.input;
            Array outData = output == IntPtr.Zero ? null : this.output;

            if (inData != null)
                switch (format.mix.sample) {
                    case XtSample.UInt8:
                        Marshal.Copy(input, (byte[])inData, 0, frames * format.inputs);
                        break;
                    case XtSample.Int16:
                        Marshal.Copy(input, (short[])inData, 0, frames * format.inputs);
                        break;
                    case XtSample.Int24:
                        Marshal.Copy(input, (byte[])inData, 0, frames * format.inputs * 3);
                        break;
                    case XtSample.Int32:
                        Marshal.Copy(input, (int[])inData, 0, frames * format.inputs);
                        break;
                    case XtSample.Float32:
                        Marshal.Copy(input, (float[])inData, 0, frames * format.inputs);
                        break;
                }
            try {
                callback(this, inData, outData, frames, time, position, timeValid, error, user);
            } catch (Exception e) {
                Environment.FailFast("Exception caught in stream callback.", e);
            }
            if (outData != null)
                switch (format.mix.sample) {
                    case XtSample.UInt8:
                        Marshal.Copy((byte[])outData, 0, output, frames * format.outputs);
                        break;
                    case XtSample.Int16:
                        Marshal.Copy((short[])outData, 0, output, frames * format.outputs);
                        break;
                    case XtSample.Int24:
                        Marshal.Copy((byte[])outData, 0, output, frames * format.outputs * 3);
                        break;
                    case XtSample.Int32:
                        Marshal.Copy((int[])outData, 0, output, frames * format.outputs);
                        break;
                    case XtSample.Float32:
                        Marshal.Copy((float[])outData, 0, output, frames * format.outputs);
                        break;
                }
        }
    }
}
