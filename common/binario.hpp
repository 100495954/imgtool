#ifndef COMMON_BINARIO_HPP
#define COMMON_BINARIO_HPP
#include <istream>

namespace binario {
  template<typename  T>
  T read_binary(std::istream & stream) {
    T value;
    //NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (!stream) {
      throw std::runtime_error("Error reading binary data from stream.");
    }
    return value;
  }


  template<typename T>
  void write_birary(std::ostream& stream, const T& value) {
    //NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

}
#endif