#include <xt/api/private/Stream.hpp>
#include <xt/private/Shared.hpp>

void
XtStream::OnRunning(XtBool running) const
{
  auto onRunning = _params.stream.onRunning;
  if(onRunning != nullptr) onRunning(this, running, _user);
}