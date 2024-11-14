add_executable(utest-img-soa
        utest-img-soa.cpp
        cutfreq-utests.cpp
resize-utests.cpp)

target_link_libraries(utest-img-soa PRIVATE imgsoa GTest::gtest_main Microsoft.GSL::GSL)