file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SRC_FILES})
file(GLOB_RECURSE INC_FILES CONFIGURE_DEPENDS include/UI/*.h)
# source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/lwgl" PREFIX "Include" FILES ${INC_FILES})

# Simple Enough UI Library
add_library(seuil STATIC ${SRC_FILES})
add_subdirectory(dependencies)
# target_sources(lwgl PRIVATE ${INC_FILES})

# target_link_libraries(seuil PRIVATE glfw)
target_link_libraries(seuil PRIVATE glad)
target_link_libraries(seuil PRIVATE glm)
target_link_libraries(seuil PRIVATE dependencies)

target_include_directories(
    seuil SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

file(GLOB_RECURSE SEUIL_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
target_sources(seuil PRIVATE ${SEUIL_RESOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/resources" PREFIX "Resources" FILES ${SEUIL_RESOURCES})

function(setup_seuil_resource_copy TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/resources"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources"
        COMMENT "Copying SEUIL resources to build directory"
    )
endfunction()