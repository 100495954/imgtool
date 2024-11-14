add_executable(utest-common
        utest-common.cpp
        info-utests.cpp
)

target_link_libraries(utest-common PRIVATE common GTest::gtest_main Microsoft.GSL::GSL)