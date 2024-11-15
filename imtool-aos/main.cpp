#include "imgaos/imageaos.hpp"
#include <cstring>
#include <iostream>
#include <gsl/gsl>
#include <vector>
#include "common/progargs.hpp"
#include "common/info.hpp"

int main(int argc, char *argv[]) {
  if (!progargsCommon::check_argc(argc)) {
    std::cerr << "Error : Wrong number of arguments; Needed minimunm 3 parameters \n";
    return -1;
  }
  gsl::span const args_view{argv, gsl::narrow<std::size_t>(argc)};
  std::vector<std::string> const args(args_view.begin(), args_view.end());
  if(!progargsCommon::args_checker(args)) {
    return -1;
  }
  if (strcmp(args[3].c_str(), "compress")==0) {
    progargsCommon::parameters_files params {.input_file = args[1], .output_file = args[2]};
    imgaos::compress(params);
  }else if (strcmp(args[3].c_str(), "resize") == 0) {
      std::vector<std::vector<imgaos::Pixel>> original;
    if (!loadPPM(args[1], original)) {return -1;}
    auto const resized = resizeImage(original, std::stoi(args[4]), std::stoi(args[5]));
    if (!savePPM(args[2], resized)) { return -1;}
    std::cout << "Resized image saved to " << args[2] << "\n";
    return 0;
  }else if (strcmp(args[3].c_str(), "maxlevel") == 0){
    imgaos::process_parametersAOS(args);
    return 0;
  }else if (strcmp(args[3].c_str(), "cutfreq") == 0) {
    imgaos::callCutfreq(args);
  }
  return 0;
}
