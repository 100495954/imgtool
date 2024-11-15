add_executable(utest-img-soa
        compress-utests.cpp
        maxlevel-utests.cpp
        cutfreq-utests.cpp
resize-utests.cpp
#        utest-img-soa.cpp
)

target_link_libraries(utest-img-soa PRIVATE imgsoa GTest::gtest_main Microsoft.GSL::GSL)