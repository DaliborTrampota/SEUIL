
add_subdirectory(freetype)

# Create Freetype::Freetype alias for msdfgen to find
if(TARGET freetype AND NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype ALIAS freetype)
endif()

set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "Disable vcpkg" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "Disable Skia" FORCE)
set(MSDF_ATLAS_NO_ARTERY_FONT ON CACHE BOOL "Disable Artery Font" FORCE)
set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "Don't build standalone exe" FORCE)
set(MSDFGEN_DISABLE_PNG ON CACHE BOOL "Disable PNG support (using stb_image instead)" FORCE)
set(MSDFGEN_DISABLE_SVG ON CACHE BOOL "Disable SVG support" FORCE)



add_subdirectory(msdf-atlas-gen)

add_library(dependencies INTERFACE)
target_include_directories(dependencies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


set_target_properties(freetype PROPERTIES FOLDER "Libraries")