add_library(common
        progargs.hpp progargs.cpp
        binario.cpp
        info.cpp
        info.hpp
        ../utest-common/utest-common.cpp
)
target_link_libraries(common PRIVATE Microsoft.GSL::GSL)