#ifndef XT_API_PRIVATE_DEVICE_HPP
#define XT_API_PRIVATE_DEVICE_HPP

#include <xt/api/public/Enums.h>
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_DEVICE(s)                                                                      \
  XtFault ShowControlPanel() override;                                                              \
  XtFault GetMix(XtBool* valid, XtMix* mix) const override;                                         \
  XtSystem GetSystem() const override { return XtSystem##s; }                                       \
  XtFault GetName(char* buffer, int32_t* size) const override;                                      \
  XtFault GetChannelCount(XtBool output, int32_t* count) const override;                            \
  XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const override;                      \
  XtFault SupportsFormat(XtFormat const* format, XtBool* supports) const override;                  \
  XtFault GetBufferSize(XtFormat const* format, XtBufferSize* size) const override;                 \
  XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const override; \
  XtFault OpenStreamCore(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream) override

struct XtDevice
{
  virtual ~XtDevice() {};
  virtual XtFault ShowControlPanel() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetMix(XtBool* valid, XtMix* mix) const = 0;
  virtual XtFault GetName(char* buffer, int32_t* size) const = 0;
  virtual XtFault GetChannelCount(XtBool output, int32_t* count) const = 0;
  virtual XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const = 0;
  virtual XtFault SupportsFormat(XtFormat const* format, XtBool* supports) const = 0;
  virtual XtFault GetBufferSize(XtFormat const* format, XtBufferSize* size) const = 0;
  virtual XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const = 0;
  XtFault OpenStream(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream);
  virtual XtFault OpenStreamCore(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream) = 0;
};

#endif // XT_API_PRIVATE_DEVICE_HPP