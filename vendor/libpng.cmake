set(PNG_SHARED OFF CACHE BOOL "Build shared lib" FORCE)
set(PNG_STATIC ON CACHE BOOL "Build static lib" FORCE)
set(PNG_TESTS OFF CACHE BOOL "Build libpng tests" FORCE)
set(PNG_BUILD_ZLIB ON CACHE BOOL "Custom zlib Location, else find_package is used" FORCE)

set(SKIP_INSTALL_EXPORT TRUE)

# zconf.h must be available when generating pnglibconf.h
set(ZLIB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/vendor/zlib ${CMAKE_SOURCE_DIR}/vendor/zlib)
set(ZLIB_LIBRARY zlib)

add_subdirectory(vendor/libpng)

# png.c:2567 compares 'unsigned int' with size_t
target_compile_options(png_static PRIVATE
    -Wno-tautological-constant-out-of-range-compare
)

target_include_directories(png_static SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/libpng
    ${CMAKE_BINARY_DIR}/vendor/libpng
)

