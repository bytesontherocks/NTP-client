
set(ProjectName "TestNtpClient")

set(THREADS_PREFER_PTHREAD_FLAG TRUE)

enable_testing()
find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(${ProjectName} test_ntp_client.cpp)
target_link_libraries(${ProjectName} PRIVATE ntp_client_lib GTest::gtest_main GTest::gmock_main ${CMAKE_THREAD_LIBS_INIT})

include(GoogleTest)
gtest_discover_tests(${ProjectName})