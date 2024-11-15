
#include "common/progargs.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace progargsCommon {
  bool check_argc(int & argc) {
    if (argc <4) {
      return false;
    }
    return true;
  }
  bool args_checker(std::vector<std::string> const &args) {
    bool const contained = pertenencia(args[3]);
    if (contained) {
       if (strcmp(args[3].c_str(), "compress") == 0 ) {
        if (args.size() != 4) {
          std::cerr << "Error : Wrong number of arguments; Needed 4 parameters\nParameters introcduced: "<<args.size()<<"\n";
          return false;
        }
         return true;
      }if (strcmp(args[3].c_str(), "resize") == 0 ) {
        if (args.size() != 6) {
          std::cerr << "Error : Wrong number of arguments; Needed 6 parameters\nParameters introcduced: "<<args.size()<<"\n";
          return false;
        }
        return true;
      }if (strcmp(args[3].c_str(), "maxlevel") == 0 ) {
        if (args.size() != 5) {
          std::cerr << "Error : Wrong number of arguments; Needed 5 parameters\nParameters introcduced: "<<args.size()<<"\n";
          return false;
        }
        return true;
      }if (strcmp(args[3].c_str(), "cutfreq") == 0 ) {
        if (args.size() != 5) {
          std::cerr << "Error : Wrong number of arguments; Needed 5 parameters\nParameters introcduced:"<<args.size()<<"\n";
          return false;
        }
        return true;
      }
    }else {
      std::cerr << "Wrong operation introduced; "<<args[3]<<" is not a valid operation";
      return false;
    }
    return true;
  }
  bool pertenencia(std::string operation) {
    std::vector<std::string> const operations = {"compress", "resize", "cutfreq", "maxlevel", "info"};
    for (const auto& element : operations) {
      if (strcmp(element.c_str(), operation.c_str())==0) {
        return true;
      }
    }
    return false;
  }

}