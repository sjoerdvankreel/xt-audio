#ifndef XT_PRIVATE_DEVICE_HPP
#define XT_PRIVATE_DEVICE_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_DEVICE()                                                                       \
  XtFault ShowControlPanel() override;                                                              \
  XtSystem GetSystem() const override;                                                              \
  XtFault GetMix(XtBool* valid, XtMix* mix) const override;                                         \
  XtFault GetName(char* buffer, int32_t* size) const override;                                      \
  XtFault GetChannelCount(XtBool output, int32_t* count) const override;                            \
  XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const override;                      \
  XtFault SupportsFormat(XtFormat const* format, XtBool* supports) const override;                  \
  XtFault GetBufferSize(XtFormat const* format, XtBufferSize* size) const override;                 \
  XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const override; \
  XtFault OpenStream(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream) override

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
  virtual XtFault OpenStream(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream) = 0;
};

#endif // XT_PRIVATE_DEVICE_HPP