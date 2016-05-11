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

    public sealed class XtAudio : IDisposable {

        private static XtTraceCallback trace;
        private static XtNative.TraceCallbackNet netTrace;
        private static XtNative.TraceCallbackMono monoTrace;
        private static XtFatalCallback fatal;
        private static XtNative.FatalCallbackNet netFatal;
        private static XtNative.FatalCallbackMono monoFatal;

        public XtAudio(string id, IntPtr window, XtTraceCallback trace, XtFatalCallback fatal) {
            XtAudio.trace = trace;
            XtAudio.netTrace = trace == null ? null : new XtNative.TraceCallbackNet(trace);
            XtAudio.monoTrace = trace == null ? null : new XtNative.TraceCallbackMono(trace);
            XtAudio.fatal = fatal;
            XtAudio.netFatal = fatal == null ? null : new XtNative.FatalCallbackNet(fatal);
            XtAudio.monoFatal = fatal == null ? null : new XtNative.FatalCallbackMono(fatal);
            Delegate traceDelegate = Type.GetType("Mono.Runtime") != null ? (Delegate)monoTrace : netTrace;
            Delegate fatalDelegate = Type.GetType("Mono.Runtime") != null ? (Delegate)monoFatal : netFatal;
            IntPtr tracePtr = trace == null ? IntPtr.Zero : Marshal.GetFunctionPointerForDelegate(traceDelegate);
            IntPtr fatalPtr = fatal == null ? IntPtr.Zero : Marshal.GetFunctionPointerForDelegate(fatalDelegate);
            using (XtNative.Utf8Buffer buffer = new XtNative.Utf8Buffer(id))
                XtNative.XtAudioInit(buffer.ptr, window, tracePtr, fatalPtr);
        }

        public void Dispose() {
            XtNative.XtAudioTerminate();
        }

        public static bool IsWin32() {
            return XtNative.XtAudioIsWin32();
        }

        public static string GetVersion() {
            return XtNative.StringFromUtf8(XtNative.XtAudioGetVersion());
        }

        public static int GetServiceCount() {
            return XtNative.XtAudioGetServiceCount();
        }

        public static XtService GetServiceByIndex(int index) {
            return new XtService(XtNative.XtAudioGetServiceByIndex(index));
        }

        public static XtService GetServiceBySetup(XtSetup setup) {
            return new XtService(XtNative.XtAudioGetServiceBySetup(setup));
        }

        public static XtService GetServiceBySystem(XtSystem system) {
            return new XtService(XtNative.XtAudioGetServiceBySystem(system));
        }

        public static XtAttributes GetSampleAttributes(XtSample sample) {
            XtAttributes attributes = new XtAttributes();
            XtNative.XtAudioGetSampleAttributes(sample, attributes);
            return attributes;
        }
    }
}