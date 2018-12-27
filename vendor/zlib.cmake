add_subdirectory(vendor/zlib)

target_include_directories(zlib SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/zlib
    ${CMAKE_BINARY_DIR}/vendor/zlib
)
