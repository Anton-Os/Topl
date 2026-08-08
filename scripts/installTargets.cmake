set(Topl_DIR "${CMAKE_INTALL_PREFIX}/lib/Topl")

if(NOT DEFINED IGORE_NATIVELIBS)
    set(installTargets GEOLIB)
else()
    set(installTargets CORELIB GEOLIB ${engine_targets})
endif()

install(TARGETS ${installTargets}
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
