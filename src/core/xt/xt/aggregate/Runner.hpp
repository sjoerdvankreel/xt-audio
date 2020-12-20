#ifndef XT_AGGREGATE_RUNNER_HPP
#define XT_AGGREGATE_RUNNER_HPP

#include <xt/blocking/Runner.hpp>
#include <xt/aggregate/Stream.hpp>
#include <cstdint>

struct XtAggregateRunner:
public XtBlockingRunner
{
  XtAggregateStream* const _stream;
  XtAggregateRunner(XtAggregateStream* stream);

  uint32_t OnSlaveBuffer(int32_t index, XtBuffer const* buffer);
  uint32_t OnMasterBuffer(int32_t index, XtBuffer const* buffer);
  uint32_t OnBuffer(int32_t index, XtBuffer const* buffer) override final;
};

#endif // XT_AGGREGATE_RUNNER_HPP