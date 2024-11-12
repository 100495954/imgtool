#ifndef COMMON_PROGARGS_HPP
#define COMMON_PROGARGS_HPP
#include <vector>
#include <string>


namespace progargsCommon {
  constexpr int MAX_COLOR_VALUE_16BIT = 65535;

  struct parameters_files {
    std::string input_file;
    std::string output_file;
  };
}

#endif