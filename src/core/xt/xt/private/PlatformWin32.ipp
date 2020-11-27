#ifdef _WIN32
#define NOMINMAX 1
#include <Windows.h>

inline int32_t 
XtPlatform::LockIncr(int32_t volatile* dest) 
{ return InterlockedIncrement(reinterpret_cast<long volatile*>(dest)); }
inline int32_t 
XtPlatform::LockDecr(int32_t volatile* dest) 
{ return InterlockedDecrement(reinterpret_cast<long volatile*>(dest)); }
inline int32_t 
XtPlatform::Cas(int32_t volatile* dest, int32_t exch, int32_t comp)
{ return InterlockedCompareExchange(reinterpret_cast<long volatile*>(dest), exch, comp); }

#endif // _WIN32