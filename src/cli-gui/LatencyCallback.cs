using System;

namespace Xt {

    class LatencyCallback : StreamCallback {

        private long pulseSend;
        private long framesProcessed;

        internal LatencyCallback(Action<Func<string>> onError, Action<Func<string>> onMessage) :
            base("Latency", onError, onMessage) {
        }

        internal override unsafe void OnCallback(XtFormat format, bool interleaved,
             bool raw, object input, object output, int frames) {

            for (int f = 0; f < frames; f++) {
                bool pulseReceived = false;
                bool sendPulse = framesProcessed == pulseSend + format.mix.rate;
                if (sendPulse)
                    pulseSend = framesProcessed;
                byte byteValue = sendPulse ? byte.MaxValue : (byte)0;
                short shortValue = sendPulse ? short.MaxValue : (short)0;
                int intValue = sendPulse ? int.MaxValue : (int)0;
                float floatValue = sendPulse ? 1.0f : 0.0f;

                if (!raw) {
                    if (!interleaved) {
                        switch (format.mix.sample) {
                            case XtSample.UInt8:
                                ((byte[][])output)[0][f] = byteValue;
                                pulseReceived = ((byte[][])input)[0][f] < 64 || ((byte[][])input)[0][f] > 192;
                                break;
                            case XtSample.Int16:
                                ((short[][])output)[0][f] = shortValue;
                                pulseReceived = Math.Abs(((short[][])input)[0][f]) > short.MaxValue / 2;
                                break;
                            case XtSample.Int24:
                                ((byte[][])output)[0][f * 3] = byteValue;
                                pulseReceived = Math.Abs(((byte[][])input)[0][f * 3]) > byte.MaxValue / 2;
                                break;
                            case XtSample.Int32:
                                ((int[][])output)[0][f] = intValue;
                                pulseReceived = Math.Abs(((int[][])input)[0][f]) > int.MaxValue / 2;
                                break;
                            case XtSample.Float32:
                                ((float[][])output)[0][f] = floatValue;
                                pulseReceived = Math.Abs(((float[][])input)[0][f]) > 0.5;
                                break;
                        }
                    } else {
                        switch (format.mix.sample) {
                            case XtSample.UInt8:
                                ((byte[])output)[f * format.outputs] = byteValue;
                                pulseReceived = ((byte[])input)[f * format.inputs] < 64 || ((byte[])input)[f * format.inputs] > 192;
                                break;
                            case XtSample.Int16:
                                ((short[])output)[f * format.outputs] = shortValue;
                                pulseReceived = Math.Abs(((short[])input)[f * format.inputs]) > short.MaxValue / 2;
                                break;
                            case XtSample.Int24:
                                ((byte[])output)[f * format.outputs * 3] = byteValue;
                                pulseReceived = Math.Abs(((byte[])input)[f * format.inputs * 3]) > byte.MaxValue / 2;
                                break;
                            case XtSample.Int32:
                                ((int[])output)[f * format.outputs] = intValue;
                                pulseReceived = Math.Abs(((int[])input)[f * format.inputs]) > int.MaxValue / 2;
                                break;
                            case XtSample.Float32:
                                ((float[])output)[f * format.outputs] = floatValue;
                                pulseReceived = Math.Abs(((float[])input)[f * format.inputs]) > 0.5;
                                break;
                        }
                    }
                } else {
                    if (!interleaved) {
                        switch (format.mix.sample) {
                            case XtSample.UInt8:
                                ((byte**)(IntPtr)output)[0][f] = byteValue;
                                pulseReceived = ((byte**)(IntPtr)input)[0][f] < 64 || ((byte**)(IntPtr)input)[0][f] > 192;
                                break;
                            case XtSample.Int16:
                                ((short**)(IntPtr)output)[0][f] = shortValue;
                                pulseReceived = Math.Abs(((short**)(IntPtr)input)[0][f]) > short.MaxValue / 2;
                                break;
                            case XtSample.Int24:
                                ((byte**)(IntPtr)output)[0][f * 3] = byteValue;
                                pulseReceived = Math.Abs(((byte**)(IntPtr)input)[0][f * 3]) > byte.MaxValue / 2;
                                break;
                            case XtSample.Int32:
                                ((int**)(IntPtr)output)[0][f] = intValue;
                                pulseReceived = Math.Abs(((int**)(IntPtr)input)[0][f]) > int.MaxValue / 2;
                                break;
                            case XtSample.Float32:
                                ((float**)(IntPtr)output)[0][f] = floatValue;
                                pulseReceived = Math.Abs(((float**)(IntPtr)input)[0][f]) > 0.5;
                                break;
                        }
                    } else {
                        switch (format.mix.sample) {
                            case XtSample.UInt8:
                                ((byte*)(IntPtr)output)[f * format.outputs] = byteValue;
                                pulseReceived = ((byte*)(IntPtr)input)[f * format.inputs] < 64 || ((byte*)(IntPtr)input)[f * format.inputs] > 192;
                                break;
                            case XtSample.Int16:
                                ((short*)(IntPtr)output)[f * format.outputs] = shortValue;
                                pulseReceived = Math.Abs(((short*)(IntPtr)input)[f * format.inputs]) > short.MaxValue / 2;
                                break;
                            case XtSample.Int24:
                                ((byte*)(IntPtr)output)[f * format.outputs * 3] = byteValue;
                                pulseReceived = Math.Abs(((byte*)(IntPtr)input)[f * format.inputs * 3]) > byte.MaxValue / 2;
                                break;
                            case XtSample.Int32:
                                ((int*)(IntPtr)output)[f * format.outputs] = intValue;
                                pulseReceived = Math.Abs(((int*)(IntPtr)input)[f * format.inputs]) > int.MaxValue / 2;
                                break;
                            case XtSample.Float32:
                                ((float*)(IntPtr)output)[f * format.outputs] = floatValue;
                                pulseReceived = Math.Abs(((float*)(IntPtr)input)[f * format.inputs]) > 0.5;
                                break;
                        }
                    }
                }

                if (pulseReceived) {
                    onMessage(() => string.Format(
                        "Pulse received after {0} frames ({1} ms).",
                        framesProcessed - pulseSend,
                        (framesProcessed - pulseSend) * 1000.0 / format.mix.rate));
                }

                framesProcessed++;
            }
        }
    }
}
