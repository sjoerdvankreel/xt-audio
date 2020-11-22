#ifndef XT_CORE_ENUMS_HPP
#define XT_CORE_ENUMS_HPP

/** @file */

namespace Xt {

enum class Setup { ProAudio, SystemAudio, ConsumerAudio };
enum class Sample { UInt8, Int16, Int24, Int32, Float32 };
enum class Cause { Format, Service, Generic, Unknown, Endpoint };
enum class System { ALSA = 1, ASIO, JACK, WASAPI, Pulse, DSound };
enum Capabilities { CapabilitiesNone = 0x0, CapabilitiesTime = 0x1, CapabilitiesLatency = 0x2, CapabilitiesFullDuplex = 0x4, CapabilitiesChannelMask = 0x8, CapabilitiesXRunDetection = 0x10 };

} // namespace Xt
#endif // XT_CORE_ENUMS_HPP