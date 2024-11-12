#include "imgsoa/imagesoa.hpp"
#include <cstring>
#include <iostream>
#include <gsl/gsl>
#include "common/progargs.hpp"
#include "common/info.hpp"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Invalid number of arguments: " << argc << "\n";
  }
  gsl::span const args_view{argv, gsl::narrow<std::size_t>(argc)};

  std::vector<std::string> const args(args_view.begin(), args_view.end());

  if (strcmp(args[3].c_str(),"compress") == 0) {
    progargsCommon::parameters_files const params(args[1],args[2]);
    imgsoa::compress(params);
    std::cout << "función realizada con exito"<<"\n";
  }
  if (strcmp(args[3].c_str(),"info") == 0) {
    progargsCommon::parameters_files const params(args[1],args[2]);
    common::info(params);
  }
  if (strcmp(args[3].c_str(),"resize") == 0) {
    imgsoa::Image const original = imgsoa::loadPPM(args[1]);
    imgsoa::Image const resized  = imgsoa::resizeImage(original,std::stoi(args[4]),std::stoi(args[5]));
    if (!savePPM(args[2], resized)) { return 1; }
    std::cout << "Resized image saved to " << args[2] << "\n";
    return 0;
  }
  if (strcmp(args[3].c_str(),"maxlevel")==0) {
    imgsoa::process_parametersSOA(args);
    return 0;
  }
  if (strcmp(args[3].c_str(),"cutfreq")==0) {
    imgsoa::callCutfreq(args);
    return 0;
  }
}