#ifndef XT_API_PRIVATE_DEVICE_HPP
#define XT_API_PRIVATE_DEVICE_HPP

#include <xt/api/public/Enums.h>
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_DEVICE(s)                                                            \
  XtFault ShowControlPanel() override final;                                              \
  XtFault GetMix(XtBool* valid, XtMix* mix) const override final;                         \
  XtSystem GetSystem() const override final { return XtSystem##s; }                       \
  XtFault GetChannelCount(XtBool output, int32_t* count) const override final;            \
  XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const override final;      \
  XtFault SupportsFormat(XtFormat const* format, XtBool* supports) const override final;  \
  XtFault GetBufferSize(XtFormat const* format, XtBufferSize* size) const override final; \
  XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const override final
#define XT_IMPLEMENT_DEVICE_STREAM() \
  XtFault OpenStreamCore(XtDeviceStreamParams const* params, XtStream** stream) override final

struct XtDevice
{
  virtual ~XtDevice() {};
  XtFault OpenStream(XtDeviceStreamParams const* params, void* user, XtStream** stream);

  virtual XtFault ShowControlPanel() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetMix(XtBool* valid, XtMix* mix) const = 0;
  virtual XtFault GetChannelCount(XtBool output, int32_t* count) const = 0;
  virtual XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const = 0;
  virtual XtFault SupportsFormat(XtFormat const* format, XtBool* supports) const = 0;
  virtual XtFault GetBufferSize(XtFormat const* format, XtBufferSize* size) const = 0;
  virtual XtFault OpenStreamCore(XtDeviceStreamParams const* params, XtStream** stream) = 0;
  virtual XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const = 0;
};

#endif // XT_API_PRIVATE_DEVICE_HPP