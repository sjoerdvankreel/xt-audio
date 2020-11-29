#ifdef __linux__

inline int32_t
XtPlatform::LockIncr(int32_t volatile* dest)
{ return __sync_add_and_fetch(dest, 1); }
inline int32_t
XtPlatform::LockDecr(int32_t volatile* dest)
{ return __sync_sub_and_fetch(dest, 1); }
inline int32_t
XtPlatform::Cas(int32_t volatile* dest, int32_t exch, int32_t comp)
{ return __sync_val_compare_and_swap(dest, comp, exch); }

#endif // __linux__