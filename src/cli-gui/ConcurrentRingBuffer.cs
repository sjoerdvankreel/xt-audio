using System;
using System.Threading;

namespace Xt {

    class ConcurrentRingBuffer {

        internal static readonly int Size = 8 * 1024 * 1024;

        private int end = 0;
        private int begin = 0;
        private int mutating = 0;
        private readonly byte[] block = new byte[Size];

        internal ConcurrentRingBuffer() {
        }

        internal int Full() {
            int full;
            while (Interlocked.CompareExchange(ref mutating, 1, 0) != 0) ;
            full = end >= begin ? end - begin : end - begin + Size;
            if (Interlocked.CompareExchange(ref mutating, 0, 1) != 1)
                throw new InvalidOperationException();
            return full;
        }

        internal bool Read(Array data, int length) {
            bool canRead;
            while (Interlocked.CompareExchange(ref mutating, 1, 0) != 0) ;
            int full = end - begin;
            if (full < 0) full += Size;
            canRead = length <= full;
            if (canRead) {
                if (end >= begin) {
                    Buffer.BlockCopy(block, begin, data, 0, length);
                } else {
                    Buffer.BlockCopy(block, begin, data, 0, Math.Min(length, Size - begin));
                    if (Size - begin < length)
                        Buffer.BlockCopy(block, 0, data, Size - begin, length - (Size - begin));
                }
                begin += length;
                if (begin >= Size)
                    begin -= Size;
            }
            if (Interlocked.CompareExchange(ref mutating, 0, 1) != 1)
                throw new InvalidOperationException();
            return canRead;
        }

        internal bool Write(Array data, int length) {
            bool canWrite;
            while (Interlocked.CompareExchange(ref mutating, 1, 0) != 0) ;
            int full = end - begin;
            if (full < 0) full += Size;
            int empty = Size - full;
            canWrite = length <= empty;
            if (canWrite) {
                if (end >= begin) {
                    Buffer.BlockCopy(data, 0, block, end, Math.Min(length, Size - end));
                    if (Size - end < length)
                        Buffer.BlockCopy(data, Size - end, block, 0, length - (Size - end));
                } else {
                    Buffer.BlockCopy(data, 0, block, end, length);
                }
                end += length;
                if (end >= Size)
                    end -= Size;
            }
            if (Interlocked.CompareExchange(ref mutating, 0, 1) != 1)
                throw new InvalidOperationException();
            return canWrite;
        }
    }
}