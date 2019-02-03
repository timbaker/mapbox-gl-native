# scripts/generate-file-lists.js to change the source files for this target.
load_sources_list(MBGL_CORE_FILES src/core-files.json)
add_library(mbgl-core STATIC ${MBGL_CORE_FILES})

# tnb
target_compile_options(mbgl-core
    PRIVATE -std=c++14 -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-extra-semi -Wno-reserved-id-macro -Wno-exit-time-destructors -Wno-undefined-func-template -Wno-conversion -Wno-double-promotion -Wno-float-equal -Wno-documentation -Wno-documentation-unknown-command -Wno-zero-as-null-pointer-constant -Wno-old-style-cast -Wno-deprecated-dynamic-exception-spec -Wno-error
)

target_include_directories(mbgl-core
    PUBLIC include
    PRIVATE src
)

target_link_libraries(mbgl-core PRIVATE
    earcut.hpp
    eternal
    expected
    geojson-vt-cpp
    kdbush.hpp
    shelf-pack-cpp
    supercluster.hpp
    unique_resource
    wagyu
)

# FIXME: We should not leak these many
# libraries in our public interface.
target_link_libraries(mbgl-core PUBLIC
    boost
    geojson.hpp
    geometry.hpp
    icu
    optional
    polylabel
    protozero
    rapidjson
    variant
    vector-tile
)

mbgl_platform_core()

create_source_groups(mbgl-core)

set_target_properties(mbgl-core PROPERTIES FOLDER "Core")

xcode_create_scheme(TARGET mbgl-core)

initialize_xcode_cxx_build_settings(mbgl-core)
