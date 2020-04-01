using System;
using System.Runtime.InteropServices;

/* Copyright (C) 2015-2020 Sjoerd van Kreel.
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

        private static T[][] CreateNonInterleavedBuffer<T>(int channels, int elements) {
            T[][] result = new T[channels][];
            for (int i = 0; i < channels; i++)
                result[i] = new T[elements];
            return result;
        }

        private static Array CreateNonInterleavedBuffer(XtSample sample, int channels, int frames) {
            switch (sample) {
                case XtSample.UInt8:
                    return CreateNonInterleavedBuffer<byte>(channels, frames);
                case XtSample.Int16:
                    return CreateNonInterleavedBuffer<short>(channels, frames);
                case XtSample.Int24:
                    return CreateNonInterleavedBuffer<byte>(channels, frames * 3);
                case XtSample.Int32:
                    return CreateNonInterleavedBuffer<int>(channels, frames);
                case XtSample.Float32:
                    return CreateNonInterleavedBuffer<float>(channels, frames);
                default:
                    throw new ArgumentException();
            }
        }

        private static Array CreateInterleavedBuffer(XtSample sample, int channels, int frames) {
            switch (sample) {
                case XtSample.UInt8:
                    return new byte[channels * frames];
                case XtSample.Int16:
                    return new short[channels * frames];
                case XtSample.Int24:
                    return new byte[channels * 3 * frames];
                case XtSample.Int32:
                    return new int[channels * frames];
                case XtSample.Float32:
                    return new float[channels * frames];
                default:
                    throw new ArgumentException();
            }
        }

        private static void CopyInterleavedBufferFromNative(XtSample sample, IntPtr raw, Array managed, int channels, int frames) {
            switch (sample) {
                case XtSample.UInt8:
                    Marshal.Copy(raw, (byte[])managed, 0, frames * channels);
                    break;
                case XtSample.Int16:
                    Marshal.Copy(raw, (short[])managed, 0, frames * channels);
                    break;
                case XtSample.Int24:
                    Marshal.Copy(raw, (byte[])managed, 0, frames * channels * 3);
                    break;
                case XtSample.Int32:
                    Marshal.Copy(raw, (int[])managed, 0, frames * channels);
                    break;
                case XtSample.Float32:
                    Marshal.Copy(raw, (float[])managed, 0, frames * channels);
                    break;
                default:
                    throw new ArgumentException();
            }
        }

        private static void CopyInterleavedBufferToNative(XtSample sample, Array managed, IntPtr raw, int channels, int frames) {
            switch (sample) {
                case XtSample.UInt8:
                    Marshal.Copy((byte[])managed, 0, raw, frames * channels);
                    break;
                case XtSample.Int16:
                    Marshal.Copy((short[])managed, 0, raw, frames * channels);
                    break;
                case XtSample.Int24:
                    Marshal.Copy((byte[])managed, 0, raw, frames * channels * 3);
                    break;
                case XtSample.Int32:
                    Marshal.Copy((int[])managed, 0, raw, frames * channels);
                    break;
                case XtSample.Float32:
                    Marshal.Copy((float[])managed, 0, raw, frames * channels);
                    break;
                default:
                    throw new ArgumentException();
            }
        }

        private static unsafe void CopyNonInterleavedBufferFromNative(XtSample sample, IntPtr raw, Array managed, int channels, int frames) {
            void** data = (void**)raw.ToPointer();
            switch (sample) {
                case XtSample.UInt8:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(new IntPtr(data[i]), ((byte[][])managed)[i], 0, frames);
                    break;
                case XtSample.Int16:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(new IntPtr(data[i]), ((short[][])managed)[i], 0, frames);
                    break;
                case XtSample.Int24:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(new IntPtr(data[i]), ((byte[][])managed)[i], 0, frames * 3);
                    break;
                case XtSample.Int32:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(new IntPtr(data[i]), ((int[][])managed)[i], 0, frames);
                    break;
                case XtSample.Float32:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(new IntPtr(data[i]), ((float[][])managed)[i], 0, frames);
                    break;
                default:
                    throw new ArgumentException();
            }
        }

        private static unsafe void CopyNonInterleavedBufferToNative(XtSample sample, Array managed, IntPtr raw, int channels, int frames) {
            void** data = (void**)raw.ToPointer();
            switch (sample) {
                case XtSample.UInt8:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(((byte[][])managed)[i], 0, new IntPtr(data[i]), frames);
                    break;
                case XtSample.Int16:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(((short[][])managed)[i], 0, new IntPtr(data[i]), frames);
                    break;
                case XtSample.Int24:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(((byte[][])managed)[i], 0, new IntPtr(data[i]), frames * 3);
                    break;
                case XtSample.Int32:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(((int[][])managed)[i], 0, new IntPtr(data[i]), frames);
                    break;
                case XtSample.Float32:
                    for (int i = 0; i < channels; i++)
                        Marshal.Copy(((float[][])managed)[i], 0, new IntPtr(data[i]), frames);
                    break;
                default:
                    throw new ArgumentException();
            }
        }

        private readonly bool raw;
        private readonly object user;
        private readonly XtXRunCallback xRunCallback;
        private readonly XtStreamCallback streamCallback;
        private readonly XtNative.XRunCallbackWin32 win32XRunCallback;
        private readonly XtNative.XRunCallbackLinux linuxXRunCallback;
        private readonly XtNative.StreamCallbackWin32 win32StreamCallback;
        private readonly XtNative.StreamCallbackLinux linuxStreamCallback;

        internal readonly IntPtr xRunCallbackPtr;
        internal readonly IntPtr streamCallbackPtr;

        private IntPtr s;
        private Array inputInterleaved;
        private Array outputInterleaved;
        private Array inputNonInterleaved;
        private Array outputNonInterleaved;

        internal XtStream(bool raw, XtStreamCallback streamCallback, XtXRunCallback xRunCallback, object user) {
            this.raw = raw;
            this.user = user;
            this.xRunCallback = xRunCallback;
            this.streamCallback = streamCallback;
            this.win32StreamCallback = new XtNative.StreamCallbackWin32(StreamCallback);
            this.linuxStreamCallback = new XtNative.StreamCallbackLinux(StreamCallback);
            Delegate streamCallbackDelegate = Environment.OSVersion.Platform == PlatformID.Win32NT
                ? (Delegate)win32StreamCallback : linuxStreamCallback;
            streamCallbackPtr = Marshal.GetFunctionPointerForDelegate(streamCallbackDelegate);
            if (xRunCallback != null) {
                this.win32XRunCallback = new XtNative.XRunCallbackWin32(XRunCallback);
                this.linuxXRunCallback = new XtNative.XRunCallbackLinux(XRunCallback);
                Delegate xRunCallbackDelegate = Environment.OSVersion.Platform == PlatformID.Win32NT
                    ? (Delegate)win32XRunCallback : linuxXRunCallback;
                xRunCallbackPtr = Marshal.GetFunctionPointerForDelegate(xRunCallbackDelegate);
            }
        }

        public bool IsRaw() {
            return raw;
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

        public XtFormat GetFormat() {
            object native = Marshal.PtrToStructure(XtNative.XtStreamGetFormat(s), typeof(XtNative.Format));
            return ((XtNative.Format)native).FromNative();
        }

        internal void Init(IntPtr s) {
            this.s = s;
            if (!IsRaw()) {
                int frames = GetFrames();
                XtFormat format = GetFormat();
                if (IsInterleaved()) {
                    inputInterleaved = CreateInterleavedBuffer(format.mix.sample, format.inputs, frames);
                    outputInterleaved = CreateInterleavedBuffer(format.mix.sample, format.outputs, frames);
                } else {
                    inputNonInterleaved = CreateNonInterleavedBuffer(format.mix.sample, format.inputs, frames);
                    outputNonInterleaved = CreateNonInterleavedBuffer(format.mix.sample, format.outputs, frames);
                }
            }
        }

        internal void XRunCallback(int index, IntPtr user) {
            try {
                xRunCallback(index, this.user);
            } catch (Exception e) {
                if (XtAudio.trace != null)
                    XtAudio.trace(XtLevel.Fatal, string.Format("Exception caught in xrun callback: {0}.", e));
                Console.WriteLine(e);
                Console.WriteLine(e.StackTrace);
                Environment.FailFast("Exception caught in stream callback.", e);
            }
        }

        internal void StreamCallback(IntPtr stream, IntPtr input, IntPtr output,
            int frames, double time, ulong position, bool timeValid, ulong error, IntPtr u) {

            XtFormat format = GetFormat();
            bool interleaved = IsInterleaved();
            object inData = raw ? (object)input : input == IntPtr.Zero ? null : interleaved ? inputInterleaved : inputNonInterleaved;
            object outData = raw ? (object)output : output == IntPtr.Zero ? null : interleaved ? outputInterleaved : outputNonInterleaved;

            if (!raw && inData != null)
                if (interleaved)
                    CopyInterleavedBufferFromNative(format.mix.sample, input, (Array)inData, format.inputs, frames);
                else
                    CopyNonInterleavedBufferFromNative(format.mix.sample, input, (Array)inData, format.inputs, frames);

            try {
                streamCallback(this, inData, outData, frames, time, position, timeValid, error, user);
            } catch (Exception e) {
                if (XtAudio.trace != null)
                    XtAudio.trace(XtLevel.Fatal, string.Format("Exception caught in xrun callback: {0}.", e));
                Console.WriteLine(e);
                Console.WriteLine(e.StackTrace);
                Environment.FailFast("Exception caught in stream callback.", e);
            }

            if (!raw && outData != null)
                if (interleaved)
                    CopyInterleavedBufferToNative(format.mix.sample, (Array)outData, output, format.outputs, frames);
                else
                    CopyNonInterleavedBufferToNative(format.mix.sample, (Array)outData, output, format.outputs, frames);
        }
    }
}
