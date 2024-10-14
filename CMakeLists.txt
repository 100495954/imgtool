cmake_minimum_required(VERSION 3.26)
project(imgtool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror -pedantic -pedantic-errors -Wconversion -Wsign-conversion)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY "https://github.com/google/googletest"
        GIT_TAG v1.515.32
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(GLS
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG v4.0.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(GLS)

set(CMAKE_CXX_CLANG_TIDY clang-tidy;
        -format-style=file;
        -header-filter=.;)

include_directories(PUBLIC .)

add_subdirectory(common)
add_subdirectory(imgaos)
add_subdirectory(imgsoa)
add_subdirectory(imtool-aos)
add_subdirectory(imtool-soa)

enable_testing()
add_subdirectory(utest-common)
add_subdirectory(utest-imgaos)
add_subdirectory(utest-img-soa)
add_subdirectory(ftest-aos)
add_subdirectory(ftest-soa)
