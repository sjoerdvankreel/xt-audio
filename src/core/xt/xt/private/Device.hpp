#ifndef XT_PRIVATE_DEVICE_HPP
#define XT_PRIVATE_DEVICE_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_DEVICE(system)                                                                          \
  XtFault ShowControlPanel() noexcept override;                                                              \
  XtFault GetMix(XtBool* valid, XtMix* mix) const noexcept override;                                         \
  XtFault GetName(char* buffer, int32_t* size) const noexcept override;                                      \
  XtSystem GetSystem() const noexcept override { return XtSystem ## system; }                                \
  XtFault GetChannelCount(XtBool output, int32_t* count) const noexcept override;                            \
  XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const noexcept override;                      \
  XtFault SupportsFormat(XtFormat const* format, XtBool* supports) const noexcept override;                  \
  XtFault GetBufferSize(XtFormat const* format, XtBufferSize* size) const noexcept override;                 \
  XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const noexcept override; \
  XtFault OpenStream(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream) noexcept override

struct XtDevice
{
  virtual ~XtDevice() noexcept {};
  virtual XtFault ShowControlPanel() noexcept = 0;
  virtual XtSystem GetSystem() const noexcept = 0;
  virtual XtFault GetMix(XtBool* valid, XtMix* mix) const noexcept = 0;
  virtual XtFault GetName(char* buffer, int32_t* size) const noexcept = 0;
  virtual XtFault GetChannelCount(XtBool output, int32_t* count) const noexcept = 0;
  virtual XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const noexcept = 0;
  virtual XtFault SupportsFormat(XtFormat const* format, XtBool* supports) const noexcept = 0;
  virtual XtFault GetBufferSize(XtFormat const* format, XtBufferSize* size) const noexcept = 0;
  virtual XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const noexcept = 0;
  virtual XtFault OpenStream(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream) noexcept = 0;
};

#endif // XT_PRIVATE_DEVICE_HPP