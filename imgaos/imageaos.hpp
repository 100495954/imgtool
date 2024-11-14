#ifndef IMGSOA_IMAGEAOS_HPP
#define IMGSOA_IMAGEAOS_HPP

#include <cstdint>
#include <map>
#include <string>
#include "common/progargs.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace imgaos{

  struct image_size {
    int width;
    int height;
  };

  struct img_size {
    std::size_t width;
    std::size_t height;
  };

  // Declaración del struct Color
  struct Pixel {
    int r, g, b;//tambien se puede usar uint16_t

    bool operator< (const Pixel& other) const {
      return std::tie(r, g, b) < std::tie(other.r, other.g, other.b);
    }

    bool operator== (const Pixel& other) const {
      return r==other.r && g==other.g && b==other.b;
    }
  };

  // Declaración de la clase Image
  class Image {
    public:
    image_size size;
    int  max_color;
    uint32_t colors = 0;
    std::vector<Pixel> pixels;

    Image(image_size size_init, int max_c);

  };

  class Photo {
    public:
    std::string magicNumber;
    int width = 0;
    int height = 0;
    unsigned int maxColorValue = 0;
    std::vector<Pixel> pixels;
    };

  bool loadPhoto(Photo& photo, const std::string& filename);
  bool savePhoto(const Photo& photo, const std::string& filename);
  bool readHeader(std::ifstream& file, Photo& photo);
  template<typename T>
  void readPixels(std::ifstream& file, Photo& photo);

  void maxlevel(Photo& photo, unsigned int newMaxValue);

  // Función para comprimir una imagen
  void compress(progargsCommon::parameters_files & params);
  // Función para obtener los píxeles de una imagen
  void getPixels(std::string const & input_file, Image & picture);

  void imprimirExpected(std::string const & input_file);

  std::map<Pixel, uint32_t> getColors(std::vector<Pixel>& input, Image & picture);

  void printColorTable(std::map<Pixel, uint32_t> & colorlist);

  void write_dataCPPM(Image & picture, std::map<Pixel, uint32_t> & colorlist ,std::string const & output_file);

  struct PixelHash {
    static constexpr std::size_t RED_SHIFT = 16;
    static constexpr std::size_t GREEN_SHIFT = 8;

    std::size_t operator()(const Pixel& pixel) const {
      return (static_cast<std::size_t>(pixel.r) << RED_SHIFT) ^
             (static_cast<std::size_t>(pixel.g) << GREEN_SHIFT) ^
             static_cast<std::size_t>(pixel.b);
    }
  };

  // Estructura para encapsular los parámetros de la función


  struct CutFreqParams {
    std::vector<Pixel> image; // Cambiado de Pixel* a std::vector<Pixel>
    img_size size;
    int n;

    CutFreqParams(std::vector<Pixel> img, img_size size_init, int num)
      : image(std::move(img)), size(size_init), n(num) { }
  };

  std::unordered_map<Pixel, int, PixelHash> countColorFrequencyWithParams(CutFreqParams const & params);
  std::vector<Pixel> getLeastFrequentColors(std::unordered_map<Pixel, int, PixelHash> frequencyMap, int n);
  Pixel findClosestColor(Pixel const & color,
                         std::vector<std::pair<Pixel, int>> const & colorFrequency,
                         std::unordered_set<Pixel, PixelHash> const & colorsToRemove);
  void cutfreq(CutFreqParams& params);

  // Declaraciones de las funciones para leer y escribir PPM
  std::vector<Pixel> readPPM(std::string const & filename, std::size_t & width, std::size_t & height);
  void writePPM(std::string const & filename, std::vector<Pixel> const & image, std::size_t width, std::size_t height);

  // Nuevas funciones para la optimización con grid
  std::unordered_map<int, std::vector<Pixel>> buildGrid(const std::vector<Pixel>& image, int gridSize);
  Pixel findClosestColorOptimized(const Pixel& color, const std::unordered_map<int, std::vector<Pixel>>& grid, int gridSize);

  static constexpr int kMaxColorValue = 255;
  static constexpr int kMaxLineLength = 256;

  struct LineInterpolationParams {
    double x0;
    double y0;
    double x1;
    double y1;
    double x;
  };
  struct ChannelParams {
    uint16_t p1;
    uint16_t p2;
    uint16_t p3;
    uint16_t p4;
  };
  // Struct to represent an RGB pixel

  // Function declarations
  bool loadPPM(std::string const & filename, std::vector<std::vector<Pixel>> & image);
  bool savePPM(std::string const & filename, std::vector<std::vector<Pixel>> const & image);
  double interpolacionLineal(LineInterpolationParams const & params);
  Pixel bilinearInterpolate(std::vector<std::vector<Pixel>> const & original, double xcoord,
                                double ycoord);

  std::vector<std::vector<Pixel>> resizeImage(std::vector<std::vector<Pixel>> const & original,
                                              int newWidth, int newHeight);
  int clamp(int value, int minValue, int maxValue);

  void callCutfreq(std::vector<std::string> const &args);

  void process_parametersAOS(std::vector<std::string> const &args);
  void handle_maxlevel_optionAOS(std::vector<std::string> const &args, const progargsCommon::parameters_files& params);
}

#endif

