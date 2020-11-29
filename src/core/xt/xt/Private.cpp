#include <xt/Private.hpp>
#include <xt/api/private/Platform.hpp>
#include <vector>
#include <cstdarg>
#include <sstream>
#include <iostream>

// ---- local ----


 
// ---- stream ----

void 
XtBlockingStream::OnXRun() const
{
  auto xRun = _params.stream.onXRun;
  if(xRun == nullptr) return;
  if(!_aggregated) xRun(-1, _user);
  else xRun(_index, static_cast<XtAggregateContext*>(_user)->stream->_user);
}