file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SRC_FILES})
file(GLOB_RECURSE INC_FILES CONFIGURE_DEPENDS include/UI/*.h)
# source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/lwgl" PREFIX "Include" FILES ${INC_FILES})

# Simple Enough UI Library
add_library(seuil STATIC ${SRC_FILES})
# target_sources(lwgl PRIVATE ${INC_FILES})

# target_link_libraries(seuil PRIVATE glfw)
target_link_libraries(seuil PRIVATE glad)
target_link_libraries(seuil PRIVATE glm)

target_include_directories(
    seuil SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)