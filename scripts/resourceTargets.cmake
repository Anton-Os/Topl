file(GLOB image_assets ${ASSETS_DIR}/images/*.png ${ASSETS_DIR}/images/*.bmp ${ASSETS_DIR}/images/*.tiff)

# foreach(image IN ITEMS ${image_assets})
#    message(STATUS "Target image ${image}")
#    file(COPY ${image} DESTINATION ${CMAKE_BINARY_DIR}/)
#    file(COPY ${image} DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
#    file(COPY ${image} DESTINATION ${CMAKE_BINARY_DIR}/Release/)
# endforeach()

if(Rasteron_FOUND)
    message(STATUS "Performing batch texture and image generation...")

    # tools are built alongside the Rasteron submodule, see submoduleTargets.cmake
    set(rasteron_tool_dirs "${Rasteron_BINARY_DIR}" "${Rasteron_BINARY_DIR}/Debug" "${Rasteron_BINARY_DIR}/Release")
    find_file(texTool_exec "TexTool" "TexTool.exe" PATHS ${rasteron_tool_dirs} NO_DEFAULT_PATH)
    find_file(drawTool_exec "DrawTool" "DrawTool.exe" PATHS ${rasteron_tool_dirs} NO_DEFAULT_PATH)
    find_file(simTool_exec "SimTool" "SimTool.exe" PATHS ${rasteron_tool_dirs} NO_DEFAULT_PATH)

    # TODO: Dynamically generate textures
endif()