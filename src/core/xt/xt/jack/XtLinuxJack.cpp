#if XT_ENABLE_JACK
#include <xt/api/private/Platform.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/private/Shared.hpp>
#include <xt/private/Services.hpp>
#include <jack/jack.h>
#include <vector>
#include <memory>
#include <sstream>

// ---- local ----

// ---- forward ----



// ---- local ----

// ---- service ----

// ---- device ----

XtFault JackDevice::OpenStreamCore(const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) {
  
  XtFault fault;
  jack_client_t* c;
  size_t bufferFrames, sampleSize;
  std::unique_ptr<JackStream> result;

  c = jack_client_open(XtPlatform::instance->_id.c_str(), JackNoStartServer, nullptr);
  if(c == nullptr)
    return ESRCH;
  XtJackClient streamClient(c);
  
  std::vector<XtJackPort> inputs, outputs;
  if((fault = XtiJackCreatePorts(c, params->format.channels.inputs, params->format.channels.inMask, 
    JackPortIsInput, inputs)) != 0)
    return fault;
  if((fault = XtiJackCreatePorts(c, params->format.channels.outputs, params->format.channels.outMask, 
    JackPortIsOutput, outputs)) != 0)
    return fault;

  sampleSize = XtiGetSampleSize(params->format.mix.sample);
  bufferFrames = jack_get_buffer_size(streamClient.client);
  result.reset(new JackStream(std::move(streamClient), std::move(inputs), std::move(outputs), 
    params->format.channels.inputs, params->format.channels.outputs, bufferFrames, sampleSize));
  if((fault = jack_set_xrun_callback(c, &XRunCallback, result.get())) != 0)
    return fault;
  if((fault = jack_set_process_callback(c, &ProcessCallback, result.get())) != 0)
    return fault;
  *stream = result.release();
  return 0;
}

#endif // XT_ENABLE_JACK