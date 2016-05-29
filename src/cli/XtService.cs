using System;
using System.Linq;
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

    public sealed class XtService {

        private readonly IntPtr s;

        internal XtService(IntPtr s) {
            this.s = s;
        }

        public override string ToString() {
            return GetName();
        }

        public XtSystem GetSystem() {
            return XtNative.XtServiceGetSystem(s);
        }

        public string GetName() {
            return XtNative.StringFromUtf8(XtNative.XtServiceGetName(s));
        }

        public XtCapabilities GetCapabilities() {
            return XtNative.XtServiceGetCapabilities(s);
        }

        public int GetDeviceCount() {
            int count;
            XtNative.HandleError(XtNative.XtServiceGetDeviceCount(s, out count));
            return count;
        }

        public XtDevice OpenDevice(int index) {
            IntPtr d;
            XtNative.HandleError(XtNative.XtServiceOpenDevice(s, index, out d));
            return new XtDevice(d);
        }

        public XtDevice OpenDefaultDevice(bool output) {
            IntPtr d;
            XtNative.HandleError(XtNative.XtServiceOpenDefaultDevice(s, output, out d));
            return d == IntPtr.Zero ? null : new XtDevice(d);
        }

        public unsafe XtStream AggregateStream(XtDevice[] devices, XtChannels[] channels,
            double[] bufferSizes, int count, XtMix mix, bool interleaved, bool raw,
            XtDevice master, XtStreamCallback streamCallback, XtXRunCallback xRunCallback, object user) {

            IntPtr str;
            IntPtr channelsPtr = IntPtr.Zero;
            IntPtr[] ds = devices.Select(d => d.d).ToArray();
            XtStream stream = new XtStream(raw, streamCallback, xRunCallback, user);
            try {
                int size = Marshal.SizeOf(typeof(XtChannels));
                channelsPtr = Marshal.AllocHGlobal(count * size);
                for (int i = 0; i < count; i++)
                    Marshal.StructureToPtr(channels[i], new IntPtr((byte*)channelsPtr + i * size), false);
                XtNative.HandleError(XtNative.XtServiceAggregateStream(s, ds, channelsPtr, bufferSizes, count,
                    mix, interleaved, master.d, stream.streamCallbackPtr, stream.xRunCallbackPtr, IntPtr.Zero, out str));
            } finally {
                if (channelsPtr != IntPtr.Zero)
                    Marshal.FreeHGlobal(channelsPtr);
            }
            stream.Init(str);
            return stream;
        }
    }
}
