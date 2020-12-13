#if XT_ENABLE_DSOUND
#include <xt/dsound/Shared.hpp>
#include <xt/dsound/Private.hpp>

DSoundStream::
DSoundStream(bool secondary):
XtBlockingStream(secondary), _timer() { }

#endif // XT_ENABLE_DSOUND