### TESTS

add_executable(Test_MathOps demos/test/MathOps.cpp)
target_include_directories(Test_MathOps PUBLIC support/numeric)

add_executable(Test_CrazySprings demos/test/CrazySprings.cpp)
target_include_directories(Test_CrazySprings PUBLIC core support support/numeric)
target_link_libraries(Test_CrazySprings PRIVATE GEOLIB CORELIB)

add_executable(Test_OddShapes demos/test/OddShapes.cpp)
target_include_directories(Test_OddShapes PUBLIC core geometry)
target_link_libraries(Test_OddShapes PRIVATE GEOLIB)

add_executable(Test_InputTracer demos/test/InputTracer.cpp)
target_include_directories(Test_InputTracer PUBLIC support ${RESOURCE_DIR})
target_link_libraries(Test_InputTracer PRIVATE CORELIB)