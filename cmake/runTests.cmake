### TESTS

add_executable(Test_Diagnostic demos/test/Diagnostic.cpp)
target_include_directories(Test_Diagnostic PRIVATE core support geometry shaders ${RESOURCE_DIR})
target_link_libraries(Test_Diagnostic PRIVATE CORELIB)

add_executable(Test_HelloVulkan demos/test/HelloVulkan.cpp)
target_include_directories(Test_HelloVulkan PUBLIC core support geometry shaders ${RESOURCE_DIR})
target_link_libraries(Test_HelloVulkan PRIVATE CORELIB)

add_executable(Test_CrazySprings demos/test/CrazySprings.cpp)
target_include_directories(Test_CrazySprings PUBLIC core support)
target_link_libraries(Test_CrazySprings PRIVATE CORELIB)

add_executable(Test_OddShapes demos/test/OddShapes.cpp)
target_include_directories(Test_OddShapes PUBLIC support geometry)
target_link_libraries(Test_OddShapes PRIVATE GEOLIB)

add_executable(Test_InputTracer demos/test/InputTracer.cpp)
target_include_directories(Test_InputTracer PUBLIC support ${RESOURCE_DIR})
target_link_libraries(Test_InputTracer PRIVATE CORELIB)

add_executable(Test_StateSaves demos/test/StateSaves.cpp)
target_include_directories(Test_StateSaves PUBLIC core support geometry)
target_link_libraries(Test_StateSaves PRIVATE CORELIB)