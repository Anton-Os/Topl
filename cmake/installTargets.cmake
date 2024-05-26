install(TARGETS CORELIB GEOLIB ${engine_targets}
    EXPORT Topl
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FRAMEWORK DESTINATION lib
)

file(GLOB_RECURSE interfaceHeaders 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.h
)

install(FILES ${interfaceHeaders} DESTINATION include/Topl)
install(EXPORT Topl DESTINATION lib/Topl FILE ToplConfig.cmake)