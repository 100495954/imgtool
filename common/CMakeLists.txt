add_library(common
        progargs.hpp progargs.cpp
        binario.cpp
        info.cpp
        info.hpp
        ../utest-common/utest-common.cpp
        ../utest-img-soa/utest-img-soa.cpp)
target_link_libraries(common PRIVATE Microsoft.GSL::GSL)