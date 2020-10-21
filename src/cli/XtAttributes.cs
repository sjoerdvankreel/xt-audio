using System.Runtime.InteropServices;

namespace Xt
{
    [StructLayout(LayoutKind.Sequential)]
    public struct XtAttributes
    {
        public int size;
        int _isFloat;
        int _isSigned;        
        public bool isFloat { get => _isFloat != 0; set => _isFloat = value ? 0 : 1; }
        public bool isSigned { get => _isSigned != 0; set => _isSigned = value ? 0 : 1; }

        public override string ToString() => XtPrint.AttributesToString(this);
    }
}