### TESTS

add_executable(Test_MathOps demos/test/MathOps.cpp)
target_include_directories(Test_MathOps PUBLIC support)

add_executable(Test_InputTracer demos/test/InputTracer.cpp)
target_include_directories(Test_InputTracer PUBLIC support ${RESOURCE_DIR})
target_link_libraries(Test_InputTracer PRIVATE CORELIB)