file(GLOB image_assets ${ASSETS_DIR}/images/*.png ${ASSETS_DIR}/images/*.bmp ${ASSETS_DIR}/images/*.tiff)

# foreach(image IN ITEMS ${image_assets})
#    message(STATUS "Target image ${image}")
#    file(COPY ${image} DESTINATION ${CMAKE_BINARY_DIR}/)
#    file(COPY ${image} DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
#    file(COPY ${image} DESTINATION ${CMAKE_BINARY_DIR}/Release/)
# endforeach()

if(Rasteron_FOUND)
    message(STATUS "Performing batch texture and image generation...")

    find_file(texTool_exec "texTool" "texTool.exe" PATHS "${CMAKE_INSTALL_DIR}/bin")
    find_file(drawTool_exec "drawTool" "drawTool.exe" PATHS "${CMAKE_INSTALL_DIR}/bin")
    find_file(simTool_exec "simTool" "simTool.exe" PATHS "${CMAKE_INSTALL_DIR}/bin")

    # TODO: Dynamically generate textures
endif()