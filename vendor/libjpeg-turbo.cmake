set(ENABLE_SHARED OFF CACHE BOOL "Build shared libraries" FORCE)
set(ENABLE_STATIC ON CACHE BOOL "Build static libraries" FORCE)
set(WITH_JPEG8 ON CACHE BOOL "Emulate libjpeg v8 API/ABI (this makes ${CMAKE_PROJECT_NAME} backward-incompatible with libjpeg v6b)" FORCE)

# This requires the NASM assembly compiler
set(WITH_SIMD OFF CACHE BOOL "Include SIMD extensions, if available for this platform" FORCE)

set(WITH_TURBOJPEG OFF CACHE BOOL "Include the TurboJPEG API library and associated test programs" FORCE)

add_subdirectory(vendor/libjpeg-turbo)

# -Werror makes these warnings fatal
target_compile_options(rdjpgcom PRIVATE -Wno-deprecated-declarations)
target_compile_options(wrjpgcom PRIVATE -Wno-deprecated-declarations)
target_compile_options(djpeg-static PRIVATE -Wno-deprecated-declarations)
target_compile_options(jpeg-static PRIVATE -Wno-incompatible-pointer-types -Wno-deprecated-declarations)
target_compile_options(jpegtran-static PRIVATE -Wno-deprecated-declarations)
target_compile_options(cjpeg-static PRIVATE -Wno-deprecated-declarations)

target_include_directories(jpeg-static SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/libjpeg-turbo
    ${CMAKE_BINARY_DIR}/vendor/libjpeg-turbo
)

