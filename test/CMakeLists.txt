set(CMAKE_CXX_STANDARD 17)
find_package(GTest REQUIRED)
add_executable(preflow_api_test preflow_api_test.cpp)
target_link_libraries(preflow_api_test GTest::GTest GTest::Main)
