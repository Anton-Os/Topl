add_executable(Test_MathOps demos/test/MathOps.cpp)
target_include_directories(Test_MathOps PUBLIC core)

add_executable(Test_HelloVulkan demos/test/HelloVulkan.cpp)
target_include_directories(Test_HelloVulkan PUBLIC core)

add_executable(Test_CrazySprings demos/test/CrazySprings.cpp)
target_include_directories(Test_CrazySprings PUBLIC core helper)
target_link_libraries(Test_CrazySprings PRIVATE CORELIB)

add_executable(Test_OddShapes demos/test/OddShapes.cpp)
target_include_directories(Test_OddShapes PUBLIC core)
target_link_libraries(Test_OddShapes PRIVATE GEOLIB)

add_executable(Test_InputTracer demos/test/InputTracer.cpp)
target_include_directories(Test_InputTracer PUBLIC helper)
target_link_libraries(Test_InputTracer PRIVATE CORELIB)

add_executable(Test_StateSaves demos/test/StateSaves.cpp)
target_include_directories(Test_StateSaves PUBLIC helper)
target_link_libraries(Test_StateSaves PRIVATE CORELIB)