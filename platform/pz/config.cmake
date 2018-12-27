include(cmake/sqlite.cmake)

set_target_properties(sqlite PROPERTIES DEBUG_POSTFIX "d")

macro(mbgl_platform_core)

    target_sources(mbgl-core
        # Misc
        PRIVATE platform/default/include/mbgl/text/unaccent.hpp
        PRIVATE platform/default/src/mbgl/layermanager/layer_manager.cpp
        PRIVATE platform/default/src/mbgl/text/bidi.cpp
        PRIVATE platform/default/src/mbgl/text/collator.cpp
        PRIVATE platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
        PRIVATE platform/default/src/mbgl/text/unaccent.cpp
        PRIVATE platform/default/src/mbgl/util/logging_stderr.cpp
        PRIVATE platform/default/src/mbgl/util/string_stdlib.cpp
        PRIVATE platform/default/src/mbgl/util/utf.cpp

        # Image handling
        PRIVATE platform/default/src/mbgl/util/image.cpp
        PRIVATE platform/default/src/mbgl/util/jpeg_reader.cpp
        PRIVATE platform/default/src/mbgl/util/png_writer.cpp
        PRIVATE platform/default/src/mbgl/util/png_reader.cpp

        # Headless view
        PRIVATE platform/default/include/mbgl/gl/headless_frontend.hpp
        PRIVATE platform/default/include/mbgl/gl/headless_backend.hpp
        PRIVATE platform/default/src/mbgl/gl/headless_frontend.cpp
        PRIVATE platform/default/src/mbgl/gl/headless_backend.cpp

        # Snapshotting
        PRIVATE platform/default/include/mbgl/map/map_snapshotter.hpp
        PRIVATE platform/default/src/mbgl/map/map_snapshotter.cpp

        # Thread pool
        PRIVATE platform/default/src/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/src/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/src/mbgl/util/shared_thread_pool.cpp

        # Thread
        PRIVATE platform/pz/src/thread.cpp
        PRIVATE platform/pz/src/thread_local.cpp

        PRIVATE platform/pz/include/mbgl/gl/gl_impl.hpp
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default/include
        PRIVATE platform/pz/include
        PRIVATE vendor/glfw/deps
    )

#    target_add_mason_package(mbgl-core PUBLIC libjpeg-turbo)

    target_link_libraries(mbgl-core
        PRIVATE nunicode
        PRIVATE icu
        PRIVATE jpeg-static
        PRIVATE png_static
        PRIVATE zlib
    )

    # libpng and zlib already have DEBUG_POSTFIX
    set_target_properties(mbgl-core PROPERTIES DEBUG_POSTFIX "d")
    set_target_properties(nunicode PROPERTIES DEBUG_POSTFIX "d")
    set_target_properties(icu PROPERTIES DEBUG_POSTFIX "d")
    set_target_properties(jpeg-static PROPERTIES DEBUG_POSTFIX "d")


    install(TARGETS mbgl-core icu mbgl-filesource nunicode sqlite
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

endmacro()


macro(mbgl_filesource)
    target_sources(mbgl-filesource
        # File source
        PRIVATE platform/pz/src/http_file_source.cpp

        # Database
        PRIVATE platform/default/src/mbgl/storage/sqlite3.cpp
    )

    set_target_properties(mbgl-filesource PROPERTIES DEBUG_POSTFIX "d")

    # We're not referencing any cURL symbols since we're dynamically loading it. However, we want to
    # link the library anyway since we're definitely going to load it on startup anyway.
    target_link_libraries(mbgl-filesource
        PUBLIC sqlite
        PUBLIC -Wl,--no-as-needed -lcurl -Wl,--as-needed
    )
endmacro()
