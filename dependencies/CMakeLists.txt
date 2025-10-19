
add_subdirectory(freetype)

add_library(dependencies INTERFACE)
target_include_directories(dependencies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


set_target_properties(freetype PROPERTIES FOLDER "Libraries")