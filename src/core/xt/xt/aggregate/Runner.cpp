#include <xt/aggregate/Runner.hpp>

XtAggregateRunner::
XtAggregateRunner(XtAggregateStream* stream):
XtBlockingRunner(stream) { }

uint32_t
XtAggregateRunner::OnBuffer(int32_t index, XtBuffer const* buffer)
{
  return 0;
}