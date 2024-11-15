# Executable for all unit tests with list of sources
# For example, you may have one ∗_test.cpp for each ∗.cpp in img−aos
add_executable(utest-imgaos
        compress-utests.cpp
        maxlevel-utests.cpp
cutfreq-utests.cpp
resize-utests.cpp
        maxlevel-utests.cpp)

target_link_libraries(utest-imgaos PRIVATE imgaos GTest::gtest_main Microsoft.GSL::GSL)