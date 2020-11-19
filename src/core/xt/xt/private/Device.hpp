#ifndef XT_PRIVATE_DEVICE_HPP
#define XT_PRIVATE_DEVICE_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_DEVICE(system)                                                                 \
  XtFault ShowControlPanel() override;                                                              \
  XtFault GetMix(XtBool* valid, XtMix* mix) const override;                                         \
  XtFault GetName(char* buffer, int32_t* size) const override;                                      \
  XtSystem GetSystem() const override { return XtSystem ## system; }                                \
  XtFault GetChannelCount(XtBool output, int32_t* count) const override;                            \
  XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const override;                      \
  XtFault GetBufferSize(const XtFormat* format, XtBufferSize* size) const override;                 \
  XtFault SupportsFormat(const XtFormat* format, XtBool* supports) const override;                  \
  XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const override; \
  XtFault OpenStream(const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) override

struct XtDevice
{
  virtual ~XtDevice() noexcept {};
  virtual XtFault ShowControlPanel() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetMix(XtBool* valid, XtMix* mix) const = 0;
  virtual XtFault GetName(char* buffer, int32_t* size) const = 0;
  virtual XtFault GetChannelCount(XtBool output, int32_t* count) const = 0;
  virtual XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const = 0;
  virtual XtFault SupportsFormat(const XtFormat* format, XtBool* supports) const = 0;
  virtual XtFault GetBufferSize(const XtFormat* format, XtBufferSize* size) const = 0;
  virtual XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const = 0;
  virtual XtFault OpenStream(const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) = 0;
};

#endif // XT_PRIVATE_DEVICE_HPP