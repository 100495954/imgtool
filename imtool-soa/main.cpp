#include "imgsoa/imagesoa.hpp"
#include <cstring>
#include <iostream>
#include <gsl/gsl>
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
    progargsCommon::parameters_files const params {.input_file = args[1], .output_file = args[2]};
    imgsoa::compress(params);
    return 0;
  }else if (strcmp(args[3].c_str(), "maxlevel") == 0){
    imgsoa::process_parametersSOA(args);
    return 0;
  }else if (strcmp(args[3].c_str(), "cutfreq") == 0) {
    imgsoa::callCutfreq(args);
    return 0;
  }else if (strcmp(args[3].c_str(), "resize") == 0) {
    imgsoa::Picture original;
    if (!imgsoa::loadPPM(args[1], original)) { return 1; }
    imgsoa::Picture const resized = resizeImage(original, std::stoi(args[4]), std::stoi(args[5]));
    if (!imgsoa::savePPM(args[2], resized)) { return 1; }
    std::cout << "Resized image saved to " << args[2] << "\n";
  }
  return 0;
}