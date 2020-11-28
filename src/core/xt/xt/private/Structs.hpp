#ifndef XT_PRIVATE_STRUCTS_HPP
#define XT_PRIVATE_STRUCTS_HPP

#include <vector>

struct XtBuffers
{
  std::vector<uint8_t> interleaved;
  std::vector<void*> nonInterleaved;
  std::vector<std::vector<uint8_t>> channels;
};

struct XtIOBuffers
{
  XtBuffers input;
  XtBuffers output;
};

#endif // XT_PRIVATE_STRUCTS_HPP