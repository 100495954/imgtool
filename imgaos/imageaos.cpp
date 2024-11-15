#include "imgaos/imageaos.hpp"
#include "common/binario.hpp"
#include "common/progargs.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <ranges>


namespace imgaos {

  // Implementación del constructor de Image
  Image::Image(image_size size_init, int const max_c)
      : size{size_init}, max_color(max_c), pixels(static_cast<std::vector<Pixel>::size_type>(size_init.width * size_init.height)) {}

  std::map<Pixel, uint32_t> getColors(std::vector<Pixel>& input, Image & picture){
    std::map<Pixel, uint32_t> color_table;
     // Contador explícito para los índices

    for (const auto& color : input) {
      // Solo añadir el color si no está en la tabla
      if (color_table.find(color) == color_table.end()) {
        color_table[color] = 0;
        picture.colors++;
      }
    }
    uint32_t indice = 0;
    for (auto [color, index] : color_table) {
      color_table[color] = indice;
      indice++;
    }

    return color_table;
  }

  void write_dataCPPM(Image & picture, std::map<Pixel, uint32_t> & colorlist,std::string const & output_file){
    std::ofstream file(output_file, std::ios::binary);
    if (!file.is_open()) {
      std::cerr << "Error opening output file: " << output_file << "\n";
      return;
    }
    file << "C6"<< " " << picture.size.width << " " << picture.size.height << " " << picture.max_color << " "<< picture.colors << "\n";
    constexpr int limit1 = 255;
    constexpr int limit2 = 65535;
    for (const auto& [color, index] : colorlist) {
      if (picture.max_color <= limit1) {
        auto const red = static_cast<uint8_t>(color.r);
        auto const green = static_cast<uint8_t>(color.g);
        auto const blue = static_cast<uint8_t>(color.b);
        binario::write_birary(file, red);
        binario::write_birary(file, green);
        binario::write_birary(file, blue);
      }
      else {
        binario::write_birary(file, color.r);
        binario::write_birary(file, color.g);
        binario::write_birary(file, color.b);
      }
    }
  //escribimos los pixeles
    for (const auto& pixel : picture.pixels) {
      uint32_t const index = colorlist[pixel]; // Obtén el índice del color del píxel
      if (picture.colors <= limit1) {
        binario::write_birary(file, static_cast<uint8_t>(index));
      } else if (picture.colors <= limit2) {
        binario::write_birary(file, static_cast<uint16_t>(index));
      } else {
        binario::write_birary(file, index);
      }
    }
  }

  // Implementación de compress (por completar)
  void compress(progargsCommon::parameters_files &params) {
    std::ifstream input(params.input_file, std::ios::binary);

    if (input.fail()) {
      std::cerr << "Error opening file " << params.input_file << '\n';
      return;
    }

    std::string identification;
    int width     = 0;
    int height    = 0;
    int max_color = 0;
    input >> identification >> width >> height >> max_color;
    input.get();
    if (strcmp(identification.c_str(), "P6")!=0) {
      std::cerr << "Invalid file type, the file:"<<params.input_file << " must be in PPM format\n";
      return;
    }
    Image picture({.width=width, .height=height}, max_color);
    int const color_frontera = 255;

    for(std::size_t i = 0; i < picture.pixels.size(); i++) {
      if (picture.max_color <= color_frontera) {
        picture.pixels[i].r = static_cast<int>(binario::read_binary<uint8_t>(input));
        picture.pixels[i].g = static_cast<int>(binario::read_binary<uint8_t>(input));
        picture.pixels[i].b = static_cast<int>(binario::read_binary<uint8_t>(input));
      } else {
        picture.pixels[i].r = static_cast<int>(binario::read_binary<uint16_t>(input));
        picture.pixels[i].g = static_cast<int>(binario::read_binary<uint16_t>(input));
        picture.pixels[i].b = static_cast<int>(binario::read_binary<uint16_t>(input));
      }
    }

    std::map<Pixel, uint32_t> colorlist = getColors(picture.pixels, picture);

    write_dataCPPM(picture, colorlist ,params.output_file);
  }

// Función para leer una imagen PPM y almacenarla en un vector de píxeles
std::vector<Pixel> readPPM(std::string const & filename, std::size_t & width, std::size_t & height) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) { throw std::runtime_error("Error: No se pudo abrir el archivo de entrada."); }

  std::string header;
  file >> header;
  if (header != "P6") { throw std::runtime_error("Error: Formato de archivo PPM no soportado."); }

  file >> width >> height;
  int maxVal = 0;
  file >> maxVal;
  file.ignore(1);

  std::vector<Pixel> image(width * height);
  if (maxVal <= MAX_COLOR_VALUE) {
    // Leer los píxeles si el valor máximo de color es menor o igual a 255
    for (std::size_t i = 0; i < width * height; ++i) {
      image[i].r = binario::read_binary<unsigned char>(file);
      image[i].g = binario::read_binary<unsigned char>(file);
      image[i].b = binario::read_binary<unsigned char>(file);
    }
  } else if (maxVal <= MAX_COLOR_VALUE_EXTENDED) {
    // Leer los píxeles si el valor máximo de color es menor o igual a 65535
    for (std::size_t i = 0; i < width * height; ++i) {
      image[i].r = binario::read_binary<uint16_t>(file);
      image[i].g = binario::read_binary<uint16_t>(file);
      image[i].b = binario::read_binary<uint16_t>(file);
    }
  } else {
    throw std::runtime_error("Error: Valor máximo no soportado: " + std::to_string(maxVal));
  }

  return image;
}

// Función para escribir una imagen PPM desde un vector de píxeles
void writePPM(std::string const & filename, std::vector<Pixel> const & image, std::size_t width, std::size_t height) {
  std::ofstream file(filename, std::ios::binary);
  if (!file) { throw std::runtime_error("Error: No se pudo abrir el archivo de salida."); }

  file << "P6\n" << width << " " << height << "\n" << MAX_COLOR_VALUE << "\n";
  for (auto const & pixel : image) {
    binario::write_birary(file, static_cast<unsigned char>(pixel.r));
    binario::write_birary(file, static_cast<unsigned char>(pixel.g));
    binario::write_birary(file, static_cast<unsigned char>(pixel.b));
  }
}

// Función para contar la frecuencia de colores en la imagen
std::unordered_map<Pixel, int, PixelHash> countColorFrequencyWithParams(const CutFreqParams& params) {
  std::unordered_map<Pixel, int, PixelHash> frequencyMap;

  for (const auto& pixel : params.image) {
    frequencyMap[pixel]++;
  }

  return frequencyMap;
}

// Función para obtener los n colores menos frecuentes
  std::vector<Pixel> getLeastFrequentColors(std::unordered_map<Pixel, int, PixelHash> frequencyMap, int n) {
    std::vector<std::pair<Pixel, int>> colorFrequency(frequencyMap.begin(), frequencyMap.end());

    // Use std::ranges::sort to sort the colors by frequency (menor a mayor) y luego por componentes b, g, r
    std::ranges::sort(colorFrequency, [](auto const & pixelA, auto const & pixelB) {
      if (pixelA.second != pixelB.second) {
        return pixelA.second < pixelB.second;
      }
      if (pixelA.first.b != pixelB.first.b) {
        return pixelA.first.b > pixelB.first.b;
      }
      if (pixelA.first.g != pixelB.first.g) {
        return pixelA.first.g > pixelB.first.g;
      }
      return pixelA.first.r > pixelB.first.r;
    });

    std::vector<Pixel> colorsToRemove;
    size_t const numColorsToRemove = std::min(static_cast<size_t>(n), colorFrequency.size());
    colorsToRemove.reserve(numColorsToRemove); // Pre-allocate the capacity

    for (size_t i = 0; i < numColorsToRemove; ++i) {
      colorsToRemove.push_back(colorFrequency[i].first);
    }

    return colorsToRemove;
  }

// Función para construir un grid optimizado para la búsqueda de colores
std::unordered_map<int, std::vector<Pixel>> buildGrid(const std::vector<Pixel>& image, int gridSize) {
  std::unordered_map<int, std::vector<Pixel>> grid;
  for (const auto& pixel : image) {
    int const key = ((pixel.r / gridSize) * RED_MULTIPLIER) + ((pixel.g / gridSize) * GREEN_MULTIPLIER) + (pixel.b / gridSize);
    grid[key].push_back(pixel);
  }
  return grid;
}

// Función para encontrar el color más cercano optimizado usando el grid
Pixel findClosestColorOptimized(const Pixel& color, const std::unordered_map<int, std::vector<Pixel>>& grid, int gridSize) {
  int const key = ((color.r / gridSize) * RED_MULTIPLIER) + ((color.g / gridSize) * GREEN_MULTIPLIER) + (color.b / gridSize);
  Pixel closestColor = {.r=0, .g=0, .b=0};
  double minDistanceSquared = std::numeric_limits<double>::max();

  for (int dr = -1; dr <= 1; ++dr) {
    for (int dg = -1; dg <= 1; ++dg) {
      for (int db = -1; db <= 1; ++db) {
        int const neighborKey = key + (dr * RED_MULTIPLIER) + (dg * GREEN_MULTIPLIER )+ db;
        if (grid.find(neighborKey) != grid.end()) {
          for (const auto& neighborColor : grid.at(neighborKey)) {
            double const distanceSquared = std::pow(color.r - neighborColor.r, 2) +
                                     std::pow(color.g - neighborColor.g, 2) +
                                     std::pow(color.b - neighborColor.b, 2);
            if (distanceSquared < minDistanceSquared) {
              minDistanceSquared = distanceSquared;
              closestColor = neighborColor;
            }
          }
        }
      }
    }
  }
  return closestColor;
}

// Función principal para eliminar los colores menos frecuentes de la imagen
void cutfreq(CutFreqParams& params) {
  if (params.n < 1) {
    throw std::invalid_argument("Error: Invalid cutfreq: " + std::to_string(params.n));
  }

  auto frequencyMap = countColorFrequencyWithParams(params);
  auto colorsToRemove = getLeastFrequentColors(frequencyMap, params.n);
  std::unordered_set<Pixel, PixelHash> const colorsToRemoveSet(colorsToRemove.begin(), colorsToRemove.end());

  auto grid = buildGrid(params.image, GRID_SIZE);

  std::unordered_map<Pixel, Pixel, PixelHash> replacementMap;
  for (const auto& color : colorsToRemove) {
    Pixel const closestColor = findClosestColorOptimized(color, grid, GRID_SIZE);
    replacementMap[color] = closestColor;
  }

  std::unordered_map<Pixel, Pixel, PixelHash> colorCache;

  for (auto& pixel : params.image) {
    auto cacheIter = colorCache.find(pixel);
    if (cacheIter != colorCache.end()) {
      pixel = cacheIter->second;
    } else {
      auto iter = replacementMap.find(pixel);
      if (iter != replacementMap.end()) {
        pixel = iter->second;
        colorCache[pixel] = iter->second;
      }
    }
  }
}



  bool loadPhoto(Photo& photo, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error opening file: " << filename << '\n';
      return false;
    }

    if (!readHeader(file, photo)) {
      return false;
    }

    photo.pixels.resize(static_cast<unsigned long long>(photo.width) * static_cast<unsigned long long>(photo.height));
    if (photo.maxColorValue <= MAX_COLOR_VALUE) {
      readPixels<unsigned char>(file, photo);
    } else {
      readPixels<unsigned short>(file, photo);
    }

    return true;
  }

  bool readHeader(std::ifstream& file, Photo& photo) {
    file >> photo.magicNumber >> photo.width >> photo.height >> photo.maxColorValue;
    file.ignore(1); // Ignore the newline character after maxColorValue

    if (photo.magicNumber != "P6") {
      std::cerr << "Error: Unsupported file format.\n";
      return false;
    }
    return true;
  }

  template<typename T>
  void readPixels(std::ifstream& file, Photo& photo) {
    for (auto& pixel : photo.pixels) {
      pixel.r = binario::read_binary<T>(file);
      pixel.g = binario::read_binary<T>(file);
      pixel.b = binario::read_binary<T>(file);
    }
  }

  bool savePhoto(const Photo& photo, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error opening file: " << filename << '\n';
      return false;
    }

    file << photo.magicNumber << "\n" << photo.width << " " << photo.height << "\n" << photo.maxColorValue << "\n";

    if (photo.maxColorValue <= MAX_COLOR_VALUE) {
      for (const auto& pixel : photo.pixels) {
        binario::write_birary(file, static_cast<unsigned char>(pixel.r));
        binario::write_birary(file, static_cast<unsigned char>(pixel.g));
        binario::write_birary(file, static_cast<unsigned char>(pixel.b));
      }
    } else {
      for (const auto& pixel : photo.pixels) {
        binario::write_birary(file, static_cast<unsigned short>(pixel.r));
        binario::write_birary(file, static_cast<unsigned short>(pixel.g));
        binario::write_birary(file, static_cast<unsigned short>(pixel.b));
      }
    }

    return true;
  }

  void maxlevel(Photo& photo, unsigned int newMaxValue) {
    double const scaleFactor = static_cast<double>(newMaxValue) / photo.maxColorValue;
    for (auto& pixel : photo.pixels) {
      pixel.r = static_cast<int>(pixel.r * scaleFactor);
      pixel.g = static_cast<int>(pixel.g * scaleFactor);
      pixel.b = static_cast<int>(pixel.b * scaleFactor);
    }
    photo.maxColorValue = newMaxValue;
  }


  bool loadPPM(std::string const & filename, std::vector<std::vector<Pixel>> & image) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error: Unable to open file " << filename << "\n";
      return false;
    }

    // Leer el encabezado del archivo PPM
    std::string header;
    file >> header;
    if (header != "P6") {
      std::cerr << "Error: Unsupported PPM format (" << header << ")\n";
      return false;
    }

    int width         = 0;
    int height        = 0;
    int maxColorValue = 0;
    file >> width >> height >> maxColorValue;
    file.ignore(kMaxLineLength, '\n');      // Skip to the end of the line
    if (maxColorValue != kMaxColorValue) {  // Ensure max color value is 255 for 8-bit images
      std::cerr << "Error: Unsupported max color value (" << maxColorValue << ")\n";
      return false;
    }

    // Redimensionar el vector de imagen para contener los píxeles
    image.resize(static_cast<unsigned long>(height),
                 std::vector<Pixel>(static_cast<unsigned long>(width)));

    // Leer los datos de los píxeles
    for (auto & row : image) {
      for (auto & pixel : row) {
        pixel.r = (binario::read_binary<uint8_t>(file));
        pixel.g = (binario::read_binary<uint8_t>(file));
        pixel.b = (binario::read_binary<uint8_t>(file));
      }
    }
    return true;
  }

  // Función para guardar una imagen PPM en un archivo
  bool savePPM(std::string const & filename, std::vector<std::vector<Pixel>> const & image) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error: Unable to open file " << filename << " for writing.\n";
      return false;
    }

    int const width  = static_cast<int>(image[0].size());
    int const height = static_cast<int>(image.size());
    file << "P6\n" << width << " " << height << "\n255\n";  // Ensure max color value is 255
    for (auto const & row : image) {
      for (auto const & pixel : row) {
        binario::write_birary(file, static_cast<uint8_t>(clamp(pixel.r, 0, kMaxColorValue)));
        binario::write_birary(file, static_cast<uint8_t>(clamp(pixel.g, 0, kMaxColorValue)));
        binario::write_birary(file, static_cast<uint8_t>(clamp(pixel.b, 0, kMaxColorValue)));
      }
    }
    return true;
  }

  // Función para limitar un valor entre un mínimo y un máximo
  int clamp(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(value, maxValue));
  }

  // Función para interpolación bilineal de un píxel
  Pixel bilinearInterpolate(std::vector<std::vector<Pixel>> const & original, double xcoord,
                            double ycoord) {
    // Calcular las coordenadas inferiores y superiores en x e y
    int const width_  = static_cast<int>(original[0].size());
    int const height_ = static_cast<int>(original.size());
    int xl_           = std::max(0, static_cast<int>(std::floor(xcoord)));
    int const xh_     = std::min(width_ - 1, static_cast<int>(std::ceil(xcoord)));
    int yl_           = std::max(0, static_cast<int>(std::floor(ycoord)));
    int const yh_     = std::min(height_ - 1, static_cast<int>(std::ceil(ycoord)));

    /*std::cout << "Bilinear Interpolation - xcoord: " << xcoord << ", ycoord: " << ycoord
              << ", xl: " << xl_ << ", xh: " << xh_ << ", yl: " << yl_ << ", yh: " << yh_ << "\n";*/

    // Obtener los cuatro píxeles vecinos
    Pixel const & p1_ = original[static_cast<unsigned long>(yl_)][static_cast<unsigned long>(xl_)];
    Pixel const & p2_ = original[static_cast<unsigned long>(yl_)][static_cast<unsigned long>(xh_)];
    Pixel const & p3_ = original[static_cast<unsigned long>(yh_)][static_cast<unsigned long>(xl_)];
    Pixel const & p4_ = original[static_cast<unsigned long>(yh_)][static_cast<unsigned long>(xh_)];

    // Función lambda para interpolar un canal de color
    auto interpolate = [&](double p1_val, double p2_val, double p3_val, double p4_val) {
      double const dx_ = xcoord - xl_;
      double const dy_ = ycoord - yl_;
      double const w1_ = (1 - dx_) * (1 - dy_);
      double const w2_ = dx_ * (1 - dy_);
      double const w3_ = (1 - dx_) * dy_;
      double const w4_ = dx_ * dy_;
      return static_cast<uint16_t>((p1_val * w1_) + (p2_val * w2_) + (p3_val * w3_) +
                                   (p4_val * w4_));
    };
    Pixel result{.r = interpolate(p1_.r, p2_.r, p3_.r, p4_.r),
                 .g = interpolate(p1_.g, p2_.g, p3_.g, p4_.g),
                 .b = interpolate(p1_.b, p2_.b, p3_.b, p4_.b)};

    // AÑADIDO PARA TESTEO
    /*std::cout << "Interpolated Pixel - R: " << result.r << ", G: " << result.g
              << ", B: " << result.b << "\n";*/
    return result;
  }

  // Función para redimensionar una imagen usando interpolación bilineal
  std::vector<std::vector<Pixel>> resizeImage(std::vector<std::vector<Pixel>> const & original,
                                              int newWidth, int newHeight) {
    // Obtener ancho y alto de la imagen original
    int const originalWidth  = static_cast<int>(original[0].size());
    int const originalHeight = static_cast<int>(original.size());
    // Crear el vector para la imagen redimensionada
    std::vector<std::vector<Pixel>> resized(
        static_cast<unsigned long>(newHeight),
        std::vector<Pixel>(static_cast<unsigned long>(newWidth)));

    // Recorrer cada píxel de la imagen redimensionada
    for (std::size_t y_ = 0; y_ < static_cast<std::size_t>(newHeight); ++y_) {
      for (std::size_t x_ = 0; x_ < static_cast<std::size_t>(newWidth); ++x_) {
        auto const originalX = static_cast<double>(x_) * (static_cast<double>(originalWidth - 1) /
                                                          static_cast<double>(newWidth - 1));
        auto const originalY = static_cast<double>(y_) * (static_cast<double>(originalHeight - 1) /
                                                          static_cast<double>(newHeight - 1));
        // Se obtiene el valor del píxel interpolado y se asigna a la posición actual de la imagen
        // redimensionada.
        // AÑADIDI PARA TESTEO
        /*std::cout << "Nuevo píxel en (" << x_ << ", " << y_ << ") -> Coordenadas originales: ("
                  << originalX << ", " << originalY << ")\n";*/
        // Obtener el píxel interpolado y asignarlo a la imagen redimensionada
        resized[y_][x_] = bilinearInterpolate(original, originalX, originalY);
      }
    }
    return resized;
  }

  void handle_maxlevel_optionAOS(std::vector<std::string> const &args, const progargsCommon::parameters_files& params) {
    int newMaxValue = 0;
    try {
      newMaxValue = std::stoi(args[4]);
    } catch (const std::invalid_argument&) {
      std::cerr << "Error: The fourth argument must be an integer.\n";
      return;
    }

    if (newMaxValue < 0 || newMaxValue > progargsCommon::MAX_COLOR_VALUE_16BIT) {
      std::cerr << "Error: The new max value must be between 0 and " << progargsCommon::MAX_COLOR_VALUE_16BIT << ".\n";
      return;
    }

    Photo image;
    if (!loadPhoto(image, params.input_file)) {
      return;
    }

    maxlevel(image, static_cast<unsigned int>(newMaxValue));

    if (!savePhoto(image, params.output_file)) {
      return;
    }

    std::cout << "Image processing completed successfully.\n";
  }

  void process_parametersAOS(std::vector<std::string> const &args) {
      size_t const size =5;
    if (args.size() != size) {
      std::cerr << "Error: Invalid number of extra arguments for maxlevel: " << args.size() - 3 << "\n";
      exit(-1);
    }

    int newMaxValue = 0;
    try {
      newMaxValue = std::stoi(args[4]);
    } catch (const std::invalid_argument&) {
      std::cerr << "Error: Invalid maxlevel: " << args[4] << "\n";
      exit(-1);
    }

    if (newMaxValue < 0 || newMaxValue > MAX_COLOR_VALUE_EXTENDED) {
      std::cerr << "Error: Invalid maxlevel: " << newMaxValue << "\n";
      exit(-1);
    }

    progargsCommon::parameters_files const params{.input_file=args[1], .output_file=args[2]};
    handle_maxlevel_optionAOS(args, params);
  }

  void callCutfreq(std::vector<std::string> const &args) {
    const std::string& inputFile = args[1];
    const std::string& outputFile = args[2];
    int const numColorsToRemove = std::stoi(args[4]);

    std::size_t width = 0;
    std::size_t height = 0;
    std::vector<imgaos::Pixel> image;
    image = readPPM(inputFile, width, height);
    CutFreqParams params(image, {.width = width, .height = height}, numColorsToRemove);
    std::unordered_map<imgaos::Pixel, int, imgaos::PixelHash> frequencyMap;
    // Contar frecuencia de colores
    frequencyMap = imgaos::countColorFrequencyWithParams(params);
    std::vector<Pixel> colorsToRemove;
    // Encontrar colores menos frecuentes
    colorsToRemove = getLeastFrequentColors(frequencyMap, numColorsToRemove);
    // Almacenar colores menos frecuentes en los parámetros antes de llamar a cutfreq
    params.image = image;
    // Reemplazo de colores
    cutfreq(params);
    // Escribir la imagen
    writePPM(outputFile, image, width, height);
  }
}

