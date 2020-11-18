project (xt-audio)
cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library (C API).
set (CORE_DIR "../src/core/xt")
file (GLOB_RECURSE CORE_SRC "${CORE_DIR}/*.*")
add_library (xt-core SHARED ${CORE_SRC})
target_compile_options (xt-core PRIVATE -DXT_EXPORT=1)
target_include_directories (xt-core PRIVATE ${CORE_DIR})
set_target_properties(xt-core PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../dist/core/xt")

# C++ library (C++ API).
set (CPP_DIR "../src/cpp/xt")
file (GLOB_RECURSE CPP_SRC "${CPP_DIR}/*.*")
add_library(xt-cpp INTERFACE)
target_sources(xt-cpp INTERFACE ${CPP_SRC})
target_include_directories (xt-cpp INTERFACE ${CORE_DIR})
add_library(xt-cpp_ ${CPP_SRC})
target_link_libraries (xt-cpp_ xt-core)
target_include_directories (xt-cpp_ PRIVATE ${CPP_DIR})
target_include_directories (xt-cpp_ PRIVATE ${CORE_DIR})
set_target_properties(xt-cpp_ PROPERTIES LINKER_LANGUAGE CXX)

# C++ sample program.
set (SAMPLE_DIR "../src/cpp/sample")
file (GLOB SAMPLE_SRC "${SAMPLE_DIR}/*.*")
add_executable (xt-sample ${SAMPLE_SRC})
target_link_libraries (xt-sample xt-core)
target_include_directories (xt-sample PRIVATE ${CPP_DIR})
target_include_directories (xt-sample PRIVATE ${CORE_DIR})
set_target_properties(xt-sample PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../dist/cpp/sample")

# Runtime dependencies.
if (UNIX)
  target_link_libraries (xt-core pthread)
endif ()
if (UNIX AND (NOT XT_DISABLE_JACK))
  target_link_libraries (xt-core jack)
endif ()
if (UNIX AND (NOT XT_DISABLE_ALSA))
  target_link_libraries (xt-core asound)
endif ()
if (WIN32 AND (NOT XT_DISABLE_WASAPI))
  target_link_libraries (xt-core avrt)
endif ()
if (UNIX AND (NOT XT_DISABLE_PULSE_AUDIO))
  target_link_libraries (xt-core pulse-simple pulse)
endif ()
if (WIN32 AND (NOT XT_DISABLE_DIRECT_SOUND))
  target_link_libraries (xt-core dsound dxguid winmm)
endif ()

# Source code dependencies.
if (WIN32 AND (NOT XT_DISABLE_ASIO))
  file (GLOB_RECURSE ASMJIT_SRC "${XT_ASMJIT_DIR}/*.*")
  add_library (asmjit STATIC ${ASMJIT_SRC})
  target_compile_options (asmjit PUBLIC -DASMJIT_STATIC=1)
  target_include_directories (xt-core PRIVATE ${XT_ASMJIT_DIR})
  target_link_libraries (xt-core asmjit)
endif ()

# Source code dependencies.
if (WIN32 AND (NOT XT_DISABLE_ASIO))
  file (GLOB ASIOSDK_SRC "${XT_ASIOSDK_DIR}/host/asiodrivers.cpp"
                         "${XT_ASIOSDK_DIR}/host/pc/asiolist.cpp")
  add_library (asiosdk STATIC ${ASIOSDK_SRC})
  target_include_directories (asiosdk PRIVATE ${XT_ASIOSDK_DIR}/common)
  target_include_directories (asiosdk PRIVATE ${XT_ASIOSDK_DIR}/host/pc)
  target_include_directories (xt-core PRIVATE ${XT_ASIOSDK_DIR})
  target_link_libraries (xt-core asiosdk)
endif ()

# Backend selection.
target_compile_options (xt-core PRIVATE -DXT_DISABLE_ASIO=${XT_DISABLE_ASIO})
target_compile_options (xt-core PRIVATE -DXT_DISABLE_ALSA=${XT_DISABLE_ALSA})
target_compile_options (xt-core PRIVATE -DXT_DISABLE_JACK=${XT_DISABLE_JACK})
target_compile_options (xt-core PRIVATE -DXT_DISABLE_WASAPI=${XT_DISABLE_WASAPI})
target_compile_options (xt-core PRIVATE -DXT_DISABLE_PULSE_AUDIO=${XT_DISABLE_PULSE_AUDIO})
target_compile_options (xt-core PRIVATE -DXT_DISABLE_DIRECT_SOUND=${XT_DISABLE_DIRECT_SOUND})
