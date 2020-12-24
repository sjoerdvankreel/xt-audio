#ifndef XT_CPP_ENUMS_HPP
#define XT_CPP_ENUMS_HPP

/** @file */

namespace Xt {

enum class Setup { ProAudio, SystemAudio, ConsumerAudio };
enum class Sample { UInt8, Int16, Int24, Int32, Float32 };
enum class Cause { Format, Service, Generic, Unknown, Endpoint };
enum class System { ALSA = 1, ASIO, JACK, WASAPI, Pulse, DSound };

enum EnumFlags { EnumFlagsInput = 0x1, EnumFlagsOutput = 0x2, EnumFlagsAll = EnumFlagsInput | EnumFlagsOutput };
enum ServiceCaps { ServiceCapsNone = 0x0, ServiceCapsTime = 0x1, ServiceCapsLatency = 0x2, ServiceCapsFullDuplex = 0x4, 
  ServiceCapsAggregation = 0x8, ServiceCapsChannelMask = 0x10, ServiceCapsControlPanel = 0x20, ServiceCapsXRunDetection = 0x40 };

} // namespace Xt
#endif // XT_CPP_ENUMS_HPP