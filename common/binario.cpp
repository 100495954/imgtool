#include "/home/pablo/Escritorio/Arquitectura_Computadores/imgtool/common/binario.hpp"

namespace binario {
  template<typename  T>
  T read_binary(std::istream & stream) {
    T value;
    //NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
  }


  template<typename T>
  void write_birary(std::ostream& stream, const T& value) {
    //NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

}