#ifndef COMMON_BINARIO_HPP
#define COMMON_BINARIO_HPP
#include <istream>

namespace binario {
  template<typename T>
  T read_binary(std::istream& stream);
  template<typename T>
  void write_birary(std::ostream& stream, const T& value);
}