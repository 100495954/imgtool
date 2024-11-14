#include "imgsoa/imagesoa.hpp"

#include "common/binario.hpp"
#include <cmath>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace imgsoa {

  Image::Image(image_size size_init, int max_color)
      : size{size_init}, max_color(max_color),
  red(static_cast<std::vector<uint16_t>::size_type>(size.width * size.height)),
  green(static_cast<std::vector<uint16_t>::size_type>(size.width * size.height)),
  blue(static_cast<std::vector<uint16_t>::size_type>(size.width * size.height)){ }


  std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t> getColors(Image& image) {
    std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t> color_map;

    for (std::size_t i = 0; i < image.red.size(); i++) {
      std::tuple<uint16_t, uint16_t, uint16_t> const color = std::make_tuple(image.red[i], image.green[i], image.blue[i]);

      if (color_map.find(color) == color_map.end()) {
        color_map[color] = 0;
        ++image.n_colors;
      }
    }

    uint32_t indice = 0;
    for (auto [color, index]:color_map) {
      color_map[color] = indice;
      ++indice;
    }

    return color_map;
  }

  void print_color_table(std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t>& color_list) {
    for (const auto & [color, index] : color_list) {
      std::cout << index<<"\n";
      std::cout<<"red: " << std::get<0>(color) << '\n';
      std::cout<<"green: " << std::get<1>(color) << '\n';
      std::cout<<"blue: " << std::get<2>(color) << '\n';
    }
  }

  void write_color_table(Image & image,
                  std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t> & color_list,
                  std::string & filename) {
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
      std::cerr << "Error opening output file: " << filename << "\n";
      return;
    }
    constexpr int limit1 = 255;

    for (const auto & [color, index] : color_list) {
      if (image.max_color <= limit1) {
        // Access elements of the tuple using std::get
        auto red = static_cast<uint8_t>(std::get<0>(color));
        auto green = static_cast<uint8_t>(std::get<1>(color));
        auto blue = static_cast<uint8_t>(std::get<2>(color));

        binario::write_birary(file, red);
        binario::write_birary(file, green);
        binario::write_birary(file, blue);
      } else {
        // Handle the case when max_color > 255
        uint16_t const red = std::get<0>(color);
        uint16_t const green = std::get<1>(color);
        uint16_t const blue = std::get<2>(color);

        // Write color values to the file (2 bytes per color component)
        binario::write_birary(file, red);
        binario::write_birary(file, green);
        binario::write_birary(file, blue);
      }
    }
  }

  void write_pixels(Image& image,
                    std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t>& color_list,
                    std::string& filename) {
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
      std::cerr << "Error opening output file: " << filename << "\n";
      return;
    }
    constexpr int limit1 = 255;
    constexpr int limit2 = 65535;

    for (std::size_t i = 0; i < image.red.size(); ++i) {
      // Create a tuple for the current pixel's RGB values from separate arrays
      std::tuple<uint16_t, uint16_t, uint16_t> const pixel_color = std::make_tuple(image.red[i], image.green[i], image.blue[i]);

      // Write the index corresponding to the pixel's color
      if (image.n_colors <= limit1) {
        auto index = static_cast<uint8_t>(color_list[pixel_color]);
        binario::write_birary(file, index);
      } else if (image.n_colors >= limit1 + 1 && image.n_colors <= limit2) {
        auto index = static_cast<uint16_t>(color_list[pixel_color]);
        binario::write_birary(file, index);
      } else {
        auto index = color_list[pixel_color];
        binario::write_birary(file, index);
      }
    }
  }

  void compress(progargsCommon::parameters_files params) {
    std::ifstream input(params.input_file, std::ios::binary);
    std::ofstream output(params.output_file, std::ios::binary);

    if (input.fail()) {
      std::cerr << "Error opening file " << params.input_file << '\n';
      return;
    }

    std::string idn;
    int width = 0;
    int height = 0;
    int max_color = 0;
    input >> idn >> width >> height >> max_color;
    input.get();
    image_size const size_init(width, height);

    Image image(size_init, max_color);
    int const limit = 255;

    for(std::size_t i = 0; i < image.red.size(); i++) {
      if (image.max_color <= limit) {
        image.red[i] = binario::read_binary<uint8_t>(input);
        image.green[i] = binario::read_binary<uint8_t>(input);
        image.blue[i] = binario::read_binary<uint8_t>(input);
      } else {
        image.red[i] = binario::read_binary<uint16_t>(input);
        image.green[i] = binario::read_binary<uint16_t>(input);
        image.blue[i] = binario::read_binary<uint16_t>(input);
      }
    }

    std::map<std::tuple<uint16_t, uint16_t, uint16_t>, uint32_t> color_list = getColors(image);
    output<< "C6"<< " " << image.size.width << " " << image.size.height << " " << image.max_color << " "<< image.n_colors << "\n";

    print_color_table(color_list);

    write_color_table(image, color_list, params.output_file);
    write_pixels(image, color_list, params.output_file);
  }

  bool loadPPM(std::string const & filename, Picture & image) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Error: Could not open file " << filename << '\n';
    return false;
  }

  std::string header;
  file >> header;
  if (header != "P6") {
    std::cerr << "Error: Unsupported PPM format (" << header << ")" << '\n';
    return false;
  }

  file >> image.width >> image.height;
  int maxColorValue = 0;
  file >> maxColorValue;
  file.get();

  image.r.resize(static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height));
  image.g.resize(static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height));
  image.b.resize(static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height));

  for (std::size_t i = 0; i < image.r.size(); ++i) {
    image.r[i] = binario::read_binary<uint8_t>(file);
    image.g[i] = binario::read_binary<uint8_t>(file);
    image.b[i] = binario::read_binary<uint8_t>(file);
  }

  return true;
}

bool savePPM(std::string const & filename, Picture const & image) {
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Error: Could not open file " << filename << " for writing." << '\n';
    return false;
  }

  file << "P6\n" << image.width << " " << image.height << "\n255\n";

  for (std::size_t i = 0; i < image.r.size(); ++i) {
    binario::write_birary(file, image.r[i]);
    binario::write_birary(file, image.g[i]);
    binario::write_birary(file, image.b[i]);
  }

  return true;
}

void bilinearInterpolate(const Picture& original, double xcoord, double ycoord, Pixel& color) {
  auto getClampedIndex = [](double value, std::size_t max) {
    return std::clamp(static_cast<std::size_t>(value), 0UL, max);
  };

  auto interpolate = [](double w1_, double w2_, double w3_, double w4_, uint8_t c1_, uint8_t c2_,
                        uint8_t c3_, uint8_t c4_) {
    return static_cast<uint8_t>((w1_ * c1_) + (w2_ * c2_) + (w3_ * c3_) + (w4_ * c4_));
  };

  std::size_t const xl_ = getClampedIndex(std::floor(xcoord), static_cast<size_t>(original.width - 1));
  std::size_t const xh_ = getClampedIndex(std::ceil(xcoord), static_cast<size_t>(original.width - 1));
  std::size_t const yl_ = getClampedIndex(std::floor(ycoord), static_cast<size_t>(original.height - 1));
  std::size_t const yh_ = getClampedIndex(std::ceil(ycoord), static_cast<size_t>(original.height - 1));

  std::size_t const idx1 = (yl_ * static_cast<size_t>(original.width)) + xl_;
  std::size_t const idx2 = (yl_ * static_cast<size_t>(original.width)) + xh_;
  std::size_t const idx3 = (yh_ * static_cast<size_t>(original.width)) + xl_;
  std::size_t const idx4 = (yh_ * static_cast<size_t>(original.width)) + xh_;

  double const xWeight = xcoord - static_cast<double>(xl_);
  double const yWeight = ycoord - static_cast<double>(yl_);

  color.red = interpolate((1 - xWeight) * (1 - yWeight), xWeight * (1 - yWeight),
                            (1 - xWeight) * yWeight, xWeight * yWeight,
                            original.r[idx1], original.r[idx2], original.r[idx3], original.r[idx4]);
  color.green = interpolate((1 - xWeight) * (1 - yWeight), xWeight * (1 - yWeight),
                            (1 - xWeight) * yWeight, xWeight * yWeight,
                            original.g[idx1], original.g[idx2], original.g[idx3], original.g[idx4]);
  color.blue = interpolate((1 - xWeight) * (1 - yWeight), xWeight * (1 - yWeight),
                            (1 - xWeight) * yWeight, xWeight * yWeight,
                            original.b[idx1], original.b[idx2], original.b[idx3], original.b[idx4]);
}

Picture resizeImage(Picture const & original, int newWidth, int newHeight) {
  Picture resized;
  resized.width  = newWidth;
  resized.height = newHeight;
  resized.r.resize(static_cast<std::size_t>(newWidth) * static_cast<std::size_t>(newHeight));
  resized.g.resize(static_cast<std::size_t>(newWidth) * static_cast<std::size_t>(newHeight));
  resized.b.resize(static_cast<std::size_t>(newWidth) * static_cast<std::size_t>(newHeight));

  for (std::size_t y_ = 0; y_ < static_cast<std::size_t>(newHeight); ++y_) {
    for (std::size_t x_ = 0; x_ < static_cast<std::size_t>(newWidth); ++x_) {
      auto const originalX = static_cast<double>(x_) * (static_cast<double>(original.width - 1) /
                                                        static_cast<double>(newWidth - 1));
      auto const originalY = static_cast<double>(y_) * (static_cast<double>(original.height - 1) /
                                                        static_cast<double>(newHeight - 1));

      Pixel color{};
      bilinearInterpolate(original, originalX, originalY, color);

      std::size_t const idx = (y_ * static_cast<std::size_t>(newWidth)) +
                              x_;
      resized.r[idx] = color.red;
      resized.g[idx] = color.green;
      resized.b[idx] = color.blue;
    }
  }

  return resized;
}

  bool loadPhoto(const std::string& filename, Photo& photo) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error opening file: " << filename << '\n';
      return false;
    }

    if (!readHeader(file, photo)) {
      return false;
    }

    size_t const pixelCount = static_cast<size_t>(photo.width) * static_cast<size_t>(photo.height);
    photo.red.resize(pixelCount);
    photo.green.resize(pixelCount);
    photo.blue.resize(pixelCount);

    if (photo.maxColorValue <= MAX_COLOR_VALUE_8BIT) {
      readPixels<unsigned char>(file, pixelCount, photo.red, photo.green, photo.blue);
    } else {
      readPixels<unsigned short>(file, pixelCount, photo.red, photo.green, photo.blue);
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
  void readPixels(std::ifstream& file, size_t pixelCount, std::vector<unsigned int>& red, std::vector<unsigned int>& green, std::vector<unsigned int>& blue) {
    for (size_t i = 0; i < pixelCount; ++i) {
      red[i] = binario::read_binary<T>(file);
      green[i] = binario::read_binary<T>(file);
      blue[i] = binario::read_binary<T>(file);
    }
  }

  bool savePhoto(const std::string& filename, const Photo& photo) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error opening file: " << filename << '\n';
      return false;
    }

    file << photo.magicNumber << "\n" << photo.width << " " << photo.height << "\n" << photo.maxColorValue << "\n";

    size_t const pixelCount = static_cast<size_t>(photo.width) * static_cast<size_t>(photo.height);
    if (photo.maxColorValue <= MAX_COLOR_VALUE_8BIT) {
      for (size_t i = 0; i < pixelCount; ++i) {
        binario::write_birary(file, static_cast<unsigned char>(photo.red[i]));
        binario::write_birary(file, static_cast<unsigned char>(photo.green[i]));
        binario::write_birary(file, static_cast<unsigned char>(photo.blue[i]));
      }
    } else {
      for (size_t i = 0; i < pixelCount; ++i) {
        binario::write_birary(file, static_cast<unsigned short>(photo.red[i]));
        binario::write_birary(file, static_cast<unsigned short>(photo.green[i]));
        binario::write_birary(file, static_cast<unsigned short>(photo.blue[i]));
      }
    }

    return true;
  }

  void maxlevel(Photo& photo, unsigned int newMaxValue) {
    double const scaleFactor = static_cast<double>(newMaxValue) / photo.maxColorValue;
    for (size_t i = 0; i < photo.red.size(); ++i) {
      photo.red[i] = static_cast<unsigned int>(photo.red[i] * scaleFactor);
      photo.green[i] = static_cast<unsigned int>(photo.green[i] * scaleFactor);
      photo.blue[i] = static_cast<unsigned int>(photo.blue[i] * scaleFactor);
    }
    photo.maxColorValue = newMaxValue;
  }

  void handle_maxlevel_optionSOA(std::vector<std::string> const &args, const progargsCommon::parameters_files& params) {
    if (args.size() != 5) {
      std::cerr << "Error: Invalid number of extra arguments for maxlevel: " << args.size() - 4 << '\n';
      return;
    }

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

    Photo photo;
    if (!loadPhoto(params.input_file, photo)) {
      return;
    }

    maxlevel(photo, static_cast<unsigned int>(newMaxValue));

    if (!savePhoto(params.output_file, photo)) {
      return;
    }

    std::cout << "Image processing completed successfully.\n";
  }

  void process_parametersSOA(std::vector<std::string> const &args) {
    if (args.size() < 4) {
      std::cerr << "Usage: <input_file> <output_file> <option> [<value>]\n";
      return;
    }

    progargsCommon::parameters_files const params{.input_file=args[1], .output_file=args[2]};
    const std::string& option = args[3];

    if (option == "maxlevel") {
      handle_maxlevel_optionSOA(args, params);
    }
  }

  void initializeImageSOA(ImageSOA &image, std::size_t width, std::size_t height) {
  image.width = width;
  image.height = height;
  image.r.resize(width * height);
  image.g.resize(width * height);
  image.b.resize(width * height);
}

// Lee una imagen PPM y la almacena en la estructura ImageSOA
ImageSOA readPPM(const std::string &filename, std::size_t &width, std::size_t &height) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Error: No se pudo abrir el archivo de entrada. " + filename);
  }

  std::string header;
  file >> header;  // Leer el encabezado del archivo
  if (header != "P6") {
    throw std::runtime_error("Error: Formato de archivo PPM no soportado.");
  }

  file >> width >> height; // Leer el ancho y alto de la imagen
  int max_val = 0;
  file >> max_val; // Leer el valor máximo de color
  file.ignore(1);  // Ignorar el carácter de nueva línea

  ImageSOA image;
  initializeImageSOA(image, width, height);

  if (max_val <= MAX_COLOR_VALUE) {
    // Leer los píxeles si el valor máximo de color es menor o igual a 255
    for (std::size_t i = 0; i < width * height; ++i) {
      image.r[i] = binario::read_binary<unsigned char>(file);
      image.g[i] = binario::read_binary<unsigned char>(file);
      image.b[i] = binario::read_binary<unsigned char>(file);
    }
  } else if (max_val <= static_cast<int>(MAX_COLOR_VALUE_8BIT)) {
    // Leer los píxeles si el valor máximo de color es menor o igual a 65535
    for (std::size_t i = 0; i < width * height; ++i) {
      image.r[i] = binario::read_binary<uint16_t>(file);
      image.g[i] = binario::read_binary<uint16_t>(file);
      image.b[i] = binario::read_binary<uint16_t>(file);
    }
  } else {
    throw std::runtime_error("Error: Valor máximo no soportado: " + std::to_string(max_val));
  }

  return image;
}

// Escribe una imagen PPM desde la estructura ImageSOA
void writePPM(const std::string & filename, const ImageSOA & image, std::size_t width, std::size_t height) {
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Error: No se pudo abrir el archivo de salida. " + filename);
  }

  file << "P6\n" << width << " " << height << "\n" << MAX_COLOR_VALUE << "\n";
  for (std::size_t i = 0; i < width * height; ++i) {
    binario::write_birary(file, static_cast<unsigned char>(image.r[i]));
    binario::write_birary(file, static_cast<unsigned char>(image.g[i]));
    binario::write_birary(file, static_cast<unsigned char>(image.b[i]));
  }
}

// Función para contar la frecuencia de colores en la imagen
std::unordered_map<std::tuple<int, int, int>, int, TupleHash> countColorFrequencySOA(CutFreqParamsSOA const & params) {
  std::unordered_map<std::tuple<int, int, int>, int, TupleHash> frequencyMap;

  for (std::size_t i = 0; i < params.image->width * params.image->height; ++i) {
    auto color = std::make_tuple(params.image->r[i], params.image->g[i], params.image->b[i]);
    frequencyMap[color]++;
  }

  return frequencyMap;
}

// Función para obtener los n colores menos frecuentes
std::vector<std::tuple<int, int, int>> getLeastFrequentColors(std::unordered_map<std::tuple<int, int, int>, int, TupleHash> const & frequencyMap, int n) {
  std::vector<std::pair<std::tuple<int, int, int>, int>> colorFrequency(frequencyMap.begin(), frequencyMap.end());

  // Ordenar los colores por frecuencia (menor a mayor) y luego por componentes b, g, r
  std::ranges::sort(colorFrequency, [](auto const & pixelA, auto const & pixelB) {
    if (pixelA.second != pixelB.second) {
      return pixelA.second < pixelB.second;
    }
    if (std::get<2>(pixelA.first) != std::get<2>(pixelB.first)) {
      return std::get<2>(pixelA.first) > std::get<2>(pixelB.first);
    }
    if (std::get<1>(pixelA.first) != std::get<1>(pixelB.first)) {
      return std::get<1>(pixelA.first) > std::get<1>(pixelB.first);
    }
    return std::get<0>(pixelA.first) > std::get<0>(pixelB.first);
  });

  std::vector<std::tuple<int, int, int>> colorsToRemove;
  size_t const numColorsToRemove = std::min(static_cast<size_t>(n), colorFrequency.size());
    colorsToRemove.reserve(numColorsToRemove);
  for (size_t i = 0; i < numColorsToRemove; ++i) {
    colorsToRemove.push_back(colorFrequency[i].first);
  }

  return colorsToRemove;
}

// Construye un grid optimizado para la búsqueda de colores
std::unordered_map<int, std::vector<std::tuple<int, int, int>>> buildGridOptimized(const ImageSOA& image, int gridSize) {
  std::unordered_map<int, std::vector<std::tuple<int, int, int>>> grid;
  for (std::size_t i = 0; i < image.width * image.height; ++i) {
    int const rIndex = image.r[i] / gridSize;
    int const gIndex = image.g[i] / gridSize;
    int const bIndex = image.b[i] / gridSize;
    int const key =( rIndex * RED_MULTIPLIER) + (gIndex * GREEN_MULTIPLIER) + bIndex;
    grid[key].emplace_back(image.r[i], image.g[i], image.b[i]);
  }
  return grid;
}

// Encuentra el color más cercano optimizado usando el grid
std::tuple<int, int, int> findClosestColorOptimized(const std::tuple<int, int, int>& color, const std::unordered_map<int, std::vector<std::tuple<int, int, int>>>& grid, int gridSize) {
  int const rIndex = std::get<0>(color) / gridSize;
  int const gIndex = std::get<1>(color) / gridSize;
  int const bIndex = std::get<2>(color) / gridSize;
  int const key = (rIndex * RED_MULTIPLIER) + (gIndex * GREEN_MULTIPLIER) + bIndex;

  std::tuple<int, int, int> closestColor;
  double minDistance = std::numeric_limits<double>::max();

  for (int dr = -1; dr <= 1; ++dr) {
    for (int dg = -1; dg <= 1; ++dg) {
      for (int db = -1; db <= 1; ++db) {
        int const neighborKey = key + (dr * RED_MULTIPLIER) + (dg * GREEN_MULTIPLIER) + db;
        if (grid.find(neighborKey) != grid.end()) {
          for (const auto& neighborColor : grid.at(neighborKey)) {
            // Calcular la distancia euclidia entre los colores
            double const distance = std::pow(std::get<0>(color) - std::get<0>(neighborColor), 2) +
                              std::pow(std::get<1>(color) - std::get<1>(neighborColor), 2) +
                              std::pow(std::get<2>(color) - std::get<2>(neighborColor), 2);
            if (distance < minDistance) {
              minDistance = distance;
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
void cutfreq(CutFreqParamsSOA const & params) {
  if (params.n < 1) {
    throw std::invalid_argument("Error: Invalid cutfreq: " + std::to_string(params.n));
  }
  auto frequencyMap = countColorFrequencySOA(params);
  auto colorsToRemove = getLeastFrequentColors(frequencyMap, params.n);
  std::unordered_set<std::tuple<int, int, int>, TupleHash> const colorsToRemoveSet(colorsToRemove.begin(), colorsToRemove.end());
  auto grid = buildGridOptimized(*params.image, GRID_SIZE);
  std::unordered_map<std::tuple<int, int, int>, std::tuple<int, int, int>, TupleHash> replacementMap;
  for (const auto& color : colorsToRemove) {
    auto closestColor = findClosestColorOptimized(color, grid, GRID_SIZE);
    replacementMap[color] = closestColor;
  }

  for (std::size_t i = 0; i < params.image->width * params.image->height; ++i) {
    auto pixel = std::make_tuple(params.image->r[i], params.image->g[i], params.image->b[i]);
    auto iter = replacementMap.find(pixel);
    if (iter != replacementMap.end()) {
      auto newColor = iter->second;
      params.image->r[i] = std::get<0>(newColor);
      params.image->g[i] = std::get<1>(newColor);
      params.image->b[i] = std::get<2>(newColor);
    }
  }
}

  void callCutfreq(std::vector<std::string> const &args) {
    if (std::stoi(args[4]) < 1) {
      std::cerr << "Error: Operación no válida o número de colores a eliminar no válido.\n";
      return;
    }
    const std::string& inputFile = args[1];
    const std::string& outputFile = args[2];
    int const numColorsToRemove = std::stoi(args[4]);
    std::size_t width = 0;
    std::size_t height = 0;
    ImageSOA image;
    image = readPPM(inputFile, width, height);
    CutFreqParamsSOA const params(image, numColorsToRemove);
    std::unordered_map<std::tuple<int, int, int>, int, TupleHash> frequencyMap;
    frequencyMap = countColorFrequencySOA(params);
    std::vector<std::tuple<int, int, int>> colorsToRemove;
    colorsToRemove = getLeastFrequentColors(frequencyMap, numColorsToRemove);
    cutfreq(params);
    writePPM(outputFile, image, width, height);
  }
}