#ifdef _WIN32
#include <xt/api/public/XtAudio.h>
#include <xt/private/Shared.hpp>
#include <xt/Win32.hpp>
#include <vector>
#include <cstring>
#include <algorithm>

// ---- local ----

// ---- win32 ----

const char* XtwWfxChannelNames[18] = {
  "Front Left", "Front Right", "Front Center",
  "Low Frequency", "Back Left", "Back Right",
  "Front Left Of Center", "Front Right Of Center", "Back Center", 
  "Side Left", "Side Right", "Top Center",
  "Top Front Left", "Top Front Center", "Top Front Right",
  "Top Back Left", "Top Back Center", "Top Back Right"
};

// ---- win32 ----

#endif // _WIN32