#ifndef IMGSOA_IMAGESOA_HPP
#define IMGSOA_IMAGESOA_HPP

#include <cstdint>
#include <map>
#include <string>
#include "common/progargs.hpp"
#include <vector>

#include <unordered_map>

namespace imgsoa {

  constexpr unsigned int MAX_COLOR_VALUE_8BIT = 255;
  constexpr int MAX_COLOR_VALUE = 255;
  constexpr int GRID_SIZE = 8;
  constexpr int RED_MULTIPLIER = 10000;
  constexpr int GREEN_MULTIPLIER = 100;

  struct ImageSOA {
    std::vector<int> r;  // Componente rojo
    std::vector<int> g;  // Componente verde
    std::vector<int> b;  // Componente azul
    std::size_t width = 0;
    std::size_t height = 0;
  };

  // Estructura para encapsular los parámetros de la función
  struct CutFreqParamsSOA {
    ImageSOA * image;
    int n;

    CutFreqParamsSOA(ImageSOA & img, int num)
      : image(&img), n(num) { }
  };

  // Custom hash function for std::tuple<int, int, int>
  struct TupleHash {
    template <class T1, class T2, class T3>
    std::size_t operator()(const std::tuple<T1, T2, T3>& tuple) const {
      auto hash1 = std::hash<T1>{}(std::get<0>(tuple));
      auto hash2 = std::hash<T2>{}(std::get<1>(tuple));
      auto hash3 = std::hash<T3>{}(std::get<2>(tuple));
      return hash1 ^ hash2 ^ hash3;
    }
  };

  struct image_size {
    int width;
    int height;
  };

  struct Picture {
    std::vector<uint8_t> r, g, b;
    int width{}, height{};
  };

  class Image {
    public:
    image_size size;
    int max_color;
    uint32_t n_colors = 0;
    std::vector<uint16_t> red;
    std::vector<uint16_t> green;
    std::vector<uint16_t> blue;

    Image(image_size size_init, int max_color);
  };
  struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };
  class Photo {
    public:
    std::string magicNumber;
    int width = 0;
    int height = 0;
    unsigned int maxColorValue = 0;
    std::vector<unsigned int> red;
    std::vector<unsigned int> green;
    std::vector<unsigned int> blue;

  };

  bool loadPhoto(const std::string& filename, Photo& photo);
  bool savePhoto(const std::string& filename, const Photo& photo);
  bool readHeader(std::ifstream& file, Photo& photo);
  template<typename T>
  void readPixels(std::ifstream& file, size_t pixelCount, std::vector<unsigned int>& red, std::vector<unsigned int>& green, std::vector<unsigned int>& blue);


  void maxlevel(Photo& photo, unsigned int newMaxValue);

  void compress(progargsCommon::parameters_files params);

  void getPixels(Image & image, std::string & filename) ;

  std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t> getColors(Image& image);

  void print_color_table(std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t>& color_list);

  void write_color_table(Image& image, std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t>& color_list,std::string& filename);

  void write_pixels(Image& image, std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t>& color_list,std::string& filename);

  bool loadPPM(std::string const & filename, Picture & image);

  bool savePPM(std::string const & filename, Picture const & image);

  void bilinearInterpolate(const Picture& original, double xcoord, double ycoord, Pixel& color);

  Picture resizeImage(Picture const & original, int newWidth, int newHeight);

  void handle_maxlevel_optionSOA(std::vector<std::string> const &args, const progargsCommon::parameters_files& params);

  void process_parametersSOA(std::vector<std::string> const &args);

  std::unordered_map<std::tuple<int, int, int>, int, TupleHash> countColorFrequencySOA(CutFreqParamsSOA const & params);

  // Función para obtener los n colores menos frecuentes
  std::vector<std::tuple<int, int, int>> getLeastFrequentColors(std::unordered_map<std::tuple<int, int, int>, int, TupleHash> const & frequencyMap, int n);

  // Función que corta los colores menos frecuentes de la imagen
  void cutfreq(CutFreqParamsSOA const & params);

  // Declaraciones de las funciones para leer y escribir PPM
  ImageSOA readPPM(const std::string & filename, std::size_t & width, std::size_t & height);
  void writePPM(const std::string & filename, const ImageSOA & image, std::size_t width, std::size_t height);
  void initializeImageSOA(ImageSOA &image, std::size_t width, std::size_t height);
  void callCutfreq(std::vector<std::string> const &args);
  // Nuevas funciones para la optimización con grid
  std::unordered_map<int, std::vector<std::tuple<int, int, int>>> buildGridOptimized(const ImageSOA& image, int gridSize);
  std::tuple<int, int, int> findClosestColorOptimized(const std::tuple<int, int, int>& color, const std::unordered_map<int, std::vector<std::tuple<int, int, int>>>& grid, int gridSize);

}

#endif