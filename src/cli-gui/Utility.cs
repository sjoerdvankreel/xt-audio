using System;
using System.Runtime.InteropServices;

namespace Xt
{
    static class Utility
    {
        [DllImport("libc", EntryPoint = "memcpy")]
        static extern IntPtr MemCpyLinux(IntPtr dest, IntPtr src, IntPtr count);
        [DllImport("msvcrt", EntryPoint = "memcpy")]
        static extern IntPtr MemCpyWin32(IntPtr dest, IntPtr src, IntPtr count);

        internal static void MemCpy(IntPtr dest, IntPtr src, IntPtr count)
        {
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
                MemCpyWin32(dest, src, count);
            else
                MemCpyLinux(dest, src, count);
        }

        internal static Array CreateInterleavedBuffer(XtSample sample, int channels, int frames)
        {
            switch (sample)
            {
                case XtSample.UInt8:
                    return new byte[channels * frames];
                case XtSample.Int16:
                    return new short[channels * frames];
                case XtSample.Int24:
                    return new byte[channels * frames * 3];
                case XtSample.Int32:
                    return new int[channels * frames];
                case XtSample.Float32:
                    return new float[channels * frames];
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        internal static void Copy(IntPtr source, Array target, XtSample sample, int channels, int frames)
        {
            switch (sample)
            {
                case XtSample.UInt8:
                    Marshal.Copy(source, (byte[])target, 0, channels * frames);
                    break;
                case XtSample.Int16:
                    Marshal.Copy(source, (short[])target, 0, channels * frames);
                    break;
                case XtSample.Int24:
                    Marshal.Copy(source, (byte[])target, 0, channels * frames * 3);
                    break;
                case XtSample.Int32:
                    Marshal.Copy(source, (int[])target, 0, channels * frames);
                    break;
                case XtSample.Float32:
                    Marshal.Copy(source, (float[])target, 0, channels * frames);
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        internal static void Copy(Array source, IntPtr target, XtSample sample, int channels, int frames)
        {
            switch (sample)
            {
                case XtSample.UInt8:
                    Marshal.Copy((byte[])source, 0, target, channels * frames);
                    break;
                case XtSample.Int16:
                    Marshal.Copy((short[])source, 0, target, channels * frames);
                    break;
                case XtSample.Int24:
                    Marshal.Copy((byte[])source, 0, target, channels * frames * 3);
                    break;
                case XtSample.Int32:
                    Marshal.Copy((int[])source, 0, target, channels * frames);
                    break;
                case XtSample.Float32:
                    Marshal.Copy((float[])source, 0, target, channels * frames);
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        internal static void Interleave(Array source, Array target, XtSample sample, int channels, int frames)
        {
            switch (sample)
            {
                case XtSample.UInt8:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((byte[])target)[f * channels + c] = ((byte[][])source)[c][f];
                    break;
                case XtSample.Int16:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((short[])target)[f * channels + c] = ((short[][])source)[c][f];
                    break;
                case XtSample.Int24:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            for (int i = 0; i < 3; i++)
                                ((byte[])target)[(f * channels + c) * 3 + i] = ((byte[][])source)[c][f * 3 + i];
                    break;
                case XtSample.Int32:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((int[])target)[f * channels + c] = ((int[][])source)[c][f];
                    break;
                case XtSample.Float32:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((float[])target)[f * channels + c] = ((float[][])source)[c][f];
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        internal static unsafe void Interleave(IntPtr source, Array target, XtSample sample, int channels, int frames)
        {
            switch (sample)
            {
                case XtSample.UInt8:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((byte[])target)[f * channels + c] = ((byte**)source)[c][f];
                    break;
                case XtSample.Int16:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((short[])target)[f * channels + c] = ((short**)source)[c][f];
                    break;
                case XtSample.Int24:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            for (int i = 0; i < 3; i++)
                                ((byte[])target)[(f * channels + c) * 3 + i] = ((byte**)source)[c][f * 3 + i];
                    break;
                case XtSample.Int32:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((int[])target)[f * channels + c] = ((int**)source)[c][f];
                    break;
                case XtSample.Float32:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((float[])target)[f * channels + c] = ((float**)source)[c][f];
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        internal static void Deinterleave(Array source, Array target, XtSample sample, int channels, int frames)
        {
            switch (sample)
            {
                case XtSample.UInt8:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((byte[][])target)[c][f] = ((byte[])source)[f * channels + c];
                    break;
                case XtSample.Int16:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((short[][])target)[c][f] = ((short[])source)[f * channels + c];
                    break;
                case XtSample.Int24:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            for (int i = 0; i < 3; i++)
                                ((byte[][])target)[c][f * 3 + i] = ((byte[])source)[(f * channels + c) * 3 + i];
                    break;
                case XtSample.Int32:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((int[][])target)[c][f] = ((int[])source)[f * channels + c];
                    break;
                case XtSample.Float32:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((float[][])target)[c][f] = ((float[])source)[f * channels + c];
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        internal static unsafe void Deinterleave(Array source, IntPtr target, XtSample sample, int channels, int frames)
        {
            switch (sample)
            {
                case XtSample.UInt8:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((byte**)target)[c][f] = ((byte[])source)[f * channels + c];
                    break;
                case XtSample.Int16:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((short**)target)[c][f] = ((short[])source)[f * channels + c];
                    break;
                case XtSample.Int24:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            for (int i = 0; i < 3; i++)
                                ((byte**)target)[c][f * 3 + i] = ((byte[])source)[(f * channels + c) * 3 + i];
                    break;
                case XtSample.Int32:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((int**)target)[c][f] = ((int[])source)[f * channels + c];
                    break;
                case XtSample.Float32:
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < channels; c++)
                            ((float**)target)[c][f] = ((float[])source)[f * channels + c];
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }
    }
}