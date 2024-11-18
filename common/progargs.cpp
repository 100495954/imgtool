
#include "common/progargs.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <ranges>
#include <algorithm>
#include <vector>

namespace progargsCommon {
  bool check_argc(int & argc) {
    return argc >= 4;
  }
  bool args_checker(std::vector<std::string> const &args) {
    bool const contained = pertenencia(args[3]);
    size_t const argc1 =5;
    size_t const argc2 =6;
    if (contained) {
       if (strcmp(args[3].c_str(), "compress") == 0 ) {
        if (args.size() != 4) {
          std::cerr << "Error : Wrong number of arguments; Needed 4 parameters\nParameters introcduced: "<<args.size()<<"\n";
          return false;
        }
         return true;
      }if (strcmp(args[3].c_str(), "resize") == 0 ) {
        if (args.size() != argc2) {
          std::cerr << "Error : Wrong number of arguments; Needed 6 parameters\nParameters introcduced: "<<args.size()<<"\n";
          return false;
        }
        return true;
      }if (strcmp(args[3].c_str(), "maxlevel") == 0 ) {
        if (args.size() != argc1) {
          std::cerr << "Error : Wrong number of arguments; Needed 5 parameters\nParameters introcduced: "<<args.size()<<"\n";
          return false;
        }
        return true;
      }if (strcmp(args[3].c_str(), "cutfreq") == 0 ) {
        if (args.size() != argc1) {
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

  bool pertenencia(std::string const & operation) {
    std::vector<std::string> const operations = {"compress", "resize", "cutfreq", "maxlevel", "info"};
    return std::ranges::any_of(operations, [&operation](const std::string& element) {
        return element == operation;
    });
  }

}