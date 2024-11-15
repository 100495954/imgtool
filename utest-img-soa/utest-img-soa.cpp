#include "common/progargs.hpp"
#include "imgsoa/imagesoa.hpp"
#include <unordered_map>
#include <common/binario.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <vector>
static constexpr int GRID_SIZE = 8;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)

// Test para verificar la generación de colores únicos en un mapa simple.
TEST(ImageTest, UniqueColorMapGeneration) {
    int const intensidad1 = 255;
    int const intensidad2 = 128;
    imgsoa::image_size const size(3, 3);
    imgsoa::Image image(size, intensidad1);

    image.red = {intensidad1, intensidad1, intensidad1, intensidad2, intensidad2, intensidad2, 0, 0, 0};
    image.green = {intensidad1, intensidad1, intensidad1, intensidad2, intensidad2, intensidad2, 0, 0, 0};
    image.blue = {intensidad1, intensidad1, intensidad1, intensidad2, intensidad2, intensidad2, 0, 0, 0};

    auto color_map = imgsoa::getColors(image);

    EXPECT_EQ(color_map.size(), 3); // 3 colores únicos: blanco, gris, negro
}

// Test para verificar manejo correcto de archivo no existente.
TEST(CompressTest, NonExistentInputFile) {
    progargsCommon::parameters_files const params = {.input_file = "nonexistent.ppm", .output_file = "output.cppm"};

    testing::internal::CaptureStderr();
    imgsoa::compress(params);
    std::string const output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(output.find("Error opening file nonexistent.ppm") != std::string::npos);
}

// Test para verificar si el archivo de salida se genera correctamente.
TEST(CompressTest, OutputFileGeneration) {
    uint8_t const valor = 0x80;
    std::string const input_filename = "test_input.ppm";
    std::ofstream input(input_filename, std::ios::binary);
    input << "P6\n1 1\n255\n";
    binario::write_birary(input,valor);
    binario::write_birary(input,valor);
    binario::write_birary(input,valor);
    input.close();

    progargsCommon::parameters_files const params = {.input_file = input_filename, .output_file = "test_output.cppm"};
    imgsoa::compress(params);

    std::ifstream output("test_output.cppm", std::ios::binary);
    ASSERT_TRUE(output.is_open());
    output.close();

    (void)std::remove(input_filename.c_str());
    (void)std::remove("test_output.cppm");
}

// Test para verificar la salida cuando max_color > 255
TEST(CompressTest, HighColorDepthHandling) {
    uint16_t const valor1 = 0xFFFF;
    uint16_t const valor2 = 0x00FF;
    uint16_t const valor3 = 0x0000;
    std::string const input_filename = "high_color.ppm";
    std::ofstream input(input_filename, std::ios::binary);
    input << "P6\n1 1\n65535\n";
    binario::write_birary(input,valor1);
    binario::write_birary(input,valor2);
    binario::write_birary(input,valor3);

    input.close();

    progargsCommon::parameters_files const params = {.input_file = input_filename, .output_file = "output.cppm"};
    imgsoa::compress(params);

    std::ifstream output("output.cppm", std::ios::binary);
    ASSERT_TRUE(output.is_open());
    output.close();

    (void)std::remove(input_filename.c_str());
    (void)std::remove("output.cppm");
}

// Test para un archivo PPM con solo un color
TEST(ImageTest, SingleColorImage) {
    int const intensidad = 128;
    int const max_color = 255;
    imgsoa::image_size const size{.width = 2, .height = 2};
    imgsoa::Image image(size, max_color);

    image.red = {intensidad, intensidad, intensidad, intensidad};
    image.green = {intensidad, intensidad, intensidad, intensidad};
    image.blue = {intensidad, intensidad, intensidad, intensidad};

    auto color_map = getColors(image);
    EXPECT_EQ(color_map.size(), 1); // Solo un color
}

// Test para archivo de entrada vacío
TEST(CompressTest, EmptyInputFile) {
    std::string const filename;
    std::ofstream file(filename);
    file.close();

    progargsCommon::parameters_files const params = {.input_file = filename, .output_file = "output.cppm"};

    testing::internal::CaptureStderr();
    imgsoa::compress(params);
    std::string const output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(output.find("Error opening file") != std::string::npos);
    (void)std::remove(filename.c_str());
}

// Test para verificar encabezado incorrecto en archivo PPM
TEST(CompressTest, IncorrectPPMHeader) {
    std::string const filename = "incorrect_header.ppm";
    std::ofstream file(filename);
    file << "P5\n2 2\n255\n";
    file.close();

    progargsCommon::parameters_files const params = {.input_file = filename, .output_file = "output.cppm"};

    testing::internal::CaptureStderr();
    imgsoa::compress(params);
    std::string const output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(output.find("Invalid file type, the file") != std::string::npos);
    (void)std::remove(filename.c_str());
}

// Test para verificar compresión de imagen de un solo píxel
TEST(CompressTest, SinglePixelImage) {
    std::string const filename = "single_pixel.ppm";
    std::ofstream file(filename, std::ios::binary);
    uint8_t const valor200 = 0xC8;
    uint8_t const valor150 = 0x96;
    uint8_t const valor100 = 0x50;
    file << "P6\n1 1\n255\n";
    binario::write_birary(file,valor200);
    binario::write_birary(file,valor150);
    binario::write_birary(file,valor100);
    file.close();

    progargsCommon::parameters_files const params = {.input_file = filename, .output_file = "output.cppm"};
    imgsoa::compress(params);

    std::ifstream output("output.cppm", std::ios::binary);
    ASSERT_TRUE(output.is_open());

    // Verificamos encabezado
    std::string header;
    output >> header;
    EXPECT_EQ(header, "C6");

    int width = 0;
    int height = 0;
    int max_color = 0;
    int n_colors = 0;
    output >> width >> height >> max_color >> n_colors;
    EXPECT_EQ(width, 1);
    EXPECT_EQ(height, 1);
    EXPECT_EQ(max_color, 255);
    EXPECT_EQ(n_colors, 1);

    output.close();
    (void)std::remove(filename.c_str());
    (void)std::remove("output.cppm");
}

// Test para verificar si los colores en max_color 255 son correctos en tabla

// Test para verificar la correcta indexación de píxeles en el archivo comprimido
TEST(ImageTest, VerifyPixelIndexingInCompressedFile) {
    int const valor = 255;
    imgsoa::image_size const size(2, 2);
    imgsoa::Image image(size, valor);

    image.red = {valor, 0, valor, 0};
    image.green = {0, valor, 0, valor};
    image.blue = {0, 0, valor, valor};

    auto color_map = getColors(image);
    std::string filename = "pixel_index_test.bin";
    write_pixels(image, color_map, filename);

    std::ifstream file(filename, std::ios::binary);
    ASSERT_TRUE(file.is_open());

    for (size_t i = 0; i < image.red.size(); ++i) {
        auto index  = binario::read_binary<uint8_t>(file);
        EXPECT_EQ(index, color_map[std::make_tuple(image.red[i], image.green[i], image.blue[i])]);
    }

    file.close();
    (void)std::remove(filename.c_str());
}


TEST(ImageSOATest, ReadPPMFunctionFileNotFound) {
    std::size_t width = 0;
    std::size_t height = 0;
    ASSERT_THROW(imgsoa::readPPM("nonexistent.ppm", width, height), std::runtime_error);
}

// Test that readPPM fails if the file has an invalid format
TEST(ImageSOATest, ReadPPMFunctionInvalidFormat) {
    std::ofstream file("invalid.ppm");
    file << "P3\n";
    file.close();
    std::size_t width = 0;
    std::size_t height = 0;
    ASSERT_THROW(imgsoa::readPPM("invalid.ppm", width, height), std::runtime_error);
}

// Tests for writePPM

// Test that writePPM successfully writes the image and the file can be opened afterward
TEST(ImageSOATest, WritePPMFunctionSuccess) {
    imgsoa::ImageSOA image;
    imgsoa::initializeImageSOA(image, 2, 2);
    image.r = {255, 0, 0, 255};
    image.g = {0, 255, 0, 255};
    image.b = {0, 0, 255, 255};

    imgsoa::writePPM("output.ppm", image, 2, 2);
    std::ifstream file("output.ppm", std::ios::binary);
    ASSERT_TRUE(file.is_open());
}

// Test that writePPM fails when trying to write to an invalid path
TEST(ImageSOATest, WritePPMFunctionFileNotWritable) {
    imgsoa::ImageSOA image;
    imgsoa::initializeImageSOA(image, 2, 2);
    image.r = {255, 0, 0, 255};
    image.g = {0, 255, 0, 255};
    image.b = {0, 0, 255, 255};

    ASSERT_THROW(imgsoa::writePPM("/invalid_path/output.ppm", image, 2, 2), std::runtime_error);
}

// Test para countColorFrequencySOA - Caso 1: Imagen con colores repetidos
TEST(ImageSOATest, CountColorFrequencySOA_RepeatedColors) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 255, 0, 0};
    image.g = {0, 0, 255, 0};
    image.b = {0, 0, 0, 255};

    imgsoa::CutFreqParamsSOA const params(image, 1);
    auto frequencyMap = countColorFrequencySOA(params);

    EXPECT_EQ((frequencyMap[{255, 0, 0}]), 2);
    EXPECT_EQ((frequencyMap[{0, 255, 0}]), 1);
    EXPECT_EQ((frequencyMap[{0, 0, 255}]), 1);
}

// Test para countColorFrequencySOA - Caso 2: Imagen sin colores repetidos
TEST(ImageSOATest, CountColorFrequencySOA_UniqueColors) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 255};
    image.g = {0, 255, 0, 255};
    image.b = {0, 0, 255, 255};

    imgsoa::CutFreqParamsSOA const params(image, 1);
    auto frequencyMap = countColorFrequencySOA(params);

    EXPECT_EQ((frequencyMap[{255, 0, 0}]), 1);
    EXPECT_EQ((frequencyMap[{0, 255, 0}]), 1);
    EXPECT_EQ((frequencyMap[{0, 0, 255}]), 1);
    EXPECT_EQ((frequencyMap[{255, 255, 255}]), 1);
}

// Test para countColorFrequencySOA - Caso 3: Imagen con un solo color
TEST(ImageSOATest, CountColorFrequencySOA_SingleColor) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 255, 255, 255};
    image.g = {0, 0, 0, 0};
    image.b = {0, 0, 0, 0};

    imgsoa::CutFreqParamsSOA const params(image, 1);
    auto frequencyMap = countColorFrequencySOA(params);

    EXPECT_EQ((frequencyMap[{255, 0, 0}]), 4);
}

// Test para getLeastFrequentColors - Caso 1: Un color menos frecuente
TEST(ImageSOATest, GetLeastFrequentColors_OneLeastFrequent) {
    std::unordered_map<std::tuple<int, int, int>, int, imgsoa::TupleHash> const frequencyMap = {
            {{255, 0, 0}, 2}, {{0, 255, 0}, 1}, {{0, 0, 255}, 1}
    };
    auto colorsToRemove = getLeastFrequentColors(frequencyMap, 1);

    EXPECT_EQ(colorsToRemove.size(), 1);
    EXPECT_EQ(colorsToRemove[0], std::make_tuple(0, 0, 255));
}

// Test para getLeastFrequentColors - Caso 2: Dos colores menos frecuentes
TEST(ImageSOATest, GetLeastFrequentColors_TwoLeastFrequent) {
    std::unordered_map<std::tuple<int, int, int>, int, imgsoa::TupleHash> const frequencyMap = {
            {{255, 0, 0}, 2}, {{0, 255, 0}, 1}, {{0, 0, 255}, 1}
    };
    auto colorsToRemove = getLeastFrequentColors(frequencyMap, 2);

    EXPECT_EQ(colorsToRemove.size(), 2);
    EXPECT_TRUE((colorsToRemove[0] == std::make_tuple(0, 0, 255) && colorsToRemove[1] == std::make_tuple(0, 255, 0)) ||
                (colorsToRemove[0] == std::make_tuple(0, 255, 0) && colorsToRemove[1] == std::make_tuple(0, 0, 255)));
}

// Test para getLeastFrequentColors - Caso 3: Todos los colores con la misma frecuencia
TEST(ImageSOATest, GetLeastFrequentColors_SameFrequency) {
    std::unordered_map<std::tuple<int, int, int>, int, imgsoa::TupleHash> const frequencyMap = {
            {{255, 0, 0}, 1}, {{0, 255, 0}, 1}, {{0, 0, 255}, 1}
    };
    auto colorsToRemove = getLeastFrequentColors(frequencyMap, 2);

    EXPECT_EQ(colorsToRemove.size(), 2);
}

// Test para buildGridOptimized - Caso 1: Imagen con colores repetidos
TEST(ImageSOATest, BuildGridOptimized_RepeatedColors) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 255, 0, 0};
    image.g = {0, 0, 255, 0};
    image.b = {0, 0, 0, 255};

    auto grid = buildGridOptimized(image, GRID_SIZE);

    EXPECT_EQ(grid.size(), 3);
}

// Test para buildGridOptimized - Caso 2: Imagen sin colores repetidos
TEST(ImageSOATest, BuildGridOptimized_UniqueColors) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 255};
    image.g = {0, 255, 0, 255};
    image.b = {0, 0, 255, 255};

    auto grid = buildGridOptimized(image, GRID_SIZE);

    EXPECT_EQ(grid.size(), 4);
}

// Test para buildGridOptimized - Caso 3: Imagen con colores en los límites del grid
TEST(ImageSOATest, BuildGridOptimized_BoundaryColors) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {0, 255};
    image.g = {0, 255};
    image.b = {0, 255};

    auto grid = buildGridOptimized(image, GRID_SIZE);

    EXPECT_EQ(grid.size(), 2);
}

// Test para findClosestColorOptimized - Caso 1: Color objetivo cercano a un color en la imagen
TEST(ImageSOATest, FindClosestColorOptimized_NearColor) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 128};
    image.g = {0, 255, 0, 128};
    image.b = {0, 0, 255, 128};

    auto grid = buildGridOptimized(image, GRID_SIZE);

    std::tuple<int, int, int> const targetColor = {127, 127, 127};
    auto closestColor =  imgsoa::findClosestColorOptimized(targetColor, grid, GRID_SIZE);

    EXPECT_EQ(closestColor, std::make_tuple(128, 128, 128));
}

// Test para findClosestColorOptimized - Caso 2: Color objetivo igual a un color en la imagen
TEST(ImageSOATest, FindClosestColorOptimized_ExactColor) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 128};
    image.g = {0, 255, 0, 128};
    image.b = {0, 0, 255, 128};

    auto grid = buildGridOptimized(image, GRID_SIZE);

    std::tuple<int, int, int> const targetColor = {255, 0, 0};
    auto closestColor = imgsoa::findClosestColorOptimized(targetColor, grid, GRID_SIZE);

    EXPECT_EQ(closestColor, std::make_tuple(255, 0, 0));
}

// Test para findClosestColorOptimized - Caso 3: Color objetivo fuera del rango de la imagen
TEST(ImageSOATest, FindClosestColorOptimized_OutOfRange) {
    imgsoa::ImageSOA image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0};
    image.g = {0, 255, 0};
    image.b = {0, 0, 255};

    auto grid = buildGridOptimized(image, GRID_SIZE);

    std::tuple<int, int, int> const targetColor = {300, 300, 300};
    auto closestColor = imgsoa::findClosestColorOptimized(targetColor, grid, GRID_SIZE);

    EXPECT_EQ(closestColor, std::make_tuple(0, 0, 0));
}

TEST(PhotoTest, LoadFunctionFileNotFound) {
    imgsoa::Photo photo;
    // Attempt to load a non-existent file and verify that the function fails without causing a segmentation fault
    ASSERT_FALSE(imgsoa::loadPhoto("nonexistent.ppm", photo));
}

// Test that loadPhoto fails if the file has an incorrect format
TEST(PhotoTest, LoadFunctionInvalidFormat) {
    std::ofstream file("invalid.ppm");
    file << "P3\n";
    file.close();
    imgsoa::Photo photo;
    // Attempt to load a file with an incorrect format and verify that the function fails without causing a segmentation fault
    ASSERT_FALSE(imgsoa::loadPhoto("invalid.ppm", photo));
}

// Verify that savePhoto correctly saves the image and that the file can be opened afterward
TEST(PhotoTest, SaveFunctionSuccess) {
    imgsoa::Photo photo;
    imgsoa::loadPhoto("test_image.ppm", photo);
    ASSERT_TRUE(imgsoa::savePhoto("output_image.ppm", photo));
    std::ifstream file("output_image.ppm", std::ios::binary);
    ASSERT_TRUE(file.is_open());
}

// Test that savePhoto fails when attempting to save to an invalid path
TEST(PhotoTest, SaveFunctionFileNotWritable) {
    imgsoa::Photo photo;
    imgsoa::loadPhoto("test_image.ppm", photo);
    // Attempt to save to an invalid location and verify that it fails without causing a segmentation fault
    ASSERT_FALSE(imgsoa::savePhoto("/invalid_path/output_image.ppm", photo));
}

// Case 1: from 255 to 128
TEST(MaxLevelTest, MaxLevelScaling_255to128) {
    imgsoa::Photo photo;
    photo.width = 2;
    photo.height = 2;
    photo.maxColorValue = 255;
    photo.red = {0, 255, 128, 64};
    photo.green = {128, 255, 128, 64};
    photo.blue = {255, 255, 128, 64};

    imgsoa::maxlevel(photo, 128);

    EXPECT_EQ(photo.red[0], 0);
    EXPECT_EQ(photo.green[0], 64);
    EXPECT_EQ(photo.blue[0], 128);
    EXPECT_EQ(photo.red[1], 128);
    EXPECT_EQ(photo.green[1], 128);
    EXPECT_EQ(photo.blue[1], 128);
    EXPECT_EQ(photo.red[2], 64);
    EXPECT_EQ(photo.green[2], 64);
    EXPECT_EQ(photo.blue[2], 64);
    EXPECT_EQ(photo.red[3], 32);
    EXPECT_EQ(photo.green[3], 32);
    EXPECT_EQ(photo.blue[3], 32);
    EXPECT_EQ(photo.maxColorValue, 128);
}

// Case 2: from 255 to 255 (same value)
TEST(MaxLevelTest, MaxLevelScaling_SameMaxValue) {
    imgsoa::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 255;
    photo.red = {0};
    photo.green = {128};
    photo.blue = {255};

    imgsoa::maxlevel(photo, 255);

    EXPECT_EQ(photo.red[0], 0);
    EXPECT_EQ(photo.green[0], 128);
    EXPECT_EQ(photo.blue[0], 255);
    EXPECT_EQ(photo.maxColorValue, 255);
}

// Case 3: from 255 to 0
TEST(MaxLevelTest, MaxLevelScaling_MaxValueZero) {
    imgsoa::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 255;
    photo.red = {0};
    photo.green = {128};
    photo.blue = {255};

    imgsoa::maxlevel(photo, 0);

    EXPECT_EQ(photo.red[0], 0);
    EXPECT_EQ(photo.green[0], 0);
    EXPECT_EQ(photo.blue[0], 0);
    EXPECT_EQ(photo.maxColorValue, 0);
}

// Case 4: from 128 to 255
TEST(MaxLevelTest, MaxLevelScaling_128to255) {
    imgsoa::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 128;
    photo.red = {0};
    photo.green = {64};
    photo.blue = {128};

    imgsoa::maxlevel(photo, 255);

    EXPECT_EQ(photo.red[0], 0);
    EXPECT_EQ(photo.green[0], 127);
    EXPECT_EQ(photo.blue[0], 255);
    EXPECT_EQ(photo.maxColorValue, 255);
}

// Case 5: from 255 to 65535
TEST(MaxLevelTest, ScaleTo65535) {
    imgsoa::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 255;
    photo.red = {0};
    photo.green = {128};
    photo.blue = {255};

    imgsoa::maxlevel(photo, 65535);

    EXPECT_EQ(photo.red[0], 0);
    EXPECT_EQ(photo.green[0], 32896);
    EXPECT_EQ(photo.blue[0], 65535);
    EXPECT_EQ(photo.maxColorValue, 65535);
}

// Case 6: from 65535 to 255
TEST(MaxLevelTest, ScaleFrom65535to255) {
    imgsoa::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 65535;
    photo.red = {0};
    photo.green = {32896};
    photo.blue = {65535};

    imgsoa::maxlevel(photo, 255);

    EXPECT_EQ(photo.red[0], 0);
    EXPECT_EQ(photo.green[0], 128);
    EXPECT_EQ(photo.blue[0], 255);
    EXPECT_EQ(photo.maxColorValue, 255);
}

TEST(ImageSOATest, LoadPPMFunctionFileNotFound) {
    imgsoa::Picture image;
    // Intenta cargar un archivo que no existe y verifica que la función falle sin causar un fallo de segmentación
    ASSERT_FALSE(loadPPM("nonexistent.ppm", image));
}

// Prueba que loadPPM falle si el archivo tiene un formato incorrecto
TEST(ImageSOATest, LoadPPMInvalidFormat) {
    // Creamos un archivo PPM con un encabezado incorrecto
    std::ofstream file("invalid_format.ppm");
    file << "P3\n2 2\n255\n";
    file.close();

    imgsoa::Picture image;
    ASSERT_FALSE(loadPPM("invalid_format.ppm", image));
}

// Tests para la función savePPM
// Verifica que savePPM guarde correctamente la imagen y que el archivo pueda abrirse después


TEST(ImageSOATest, SavePPMFunctionSuccess) {
    imgsoa::Picture image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.g = {0, 255, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.b = {0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    ASSERT_TRUE(savePPM("output.ppm", image));
    std::ifstream file("output.ppm", std::ios::binary);
    ASSERT_TRUE(file.is_open());
}

// Verifica que savePPM falle si el archivo no se puede escribir (por ejemplo, en una ruta inválida)
TEST(ImageSOATest, SavePPMFunctionFileNotWritable) {
    imgsoa::Picture image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.g = {0, 255, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.b = {0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    // Intenta guardar en una ubicación inválida y verifica que falle sin causar un fallo de segmentación
    ASSERT_FALSE(savePPM("/invalid_path/output.ppm", image));
}

// Tests para la función bilinearInterpolate
// Prueba la interpolación en el centro de una imagen 2x2
TEST(ImageSOATest, BilinearInterpolateFunctionCenter) {
    imgsoa::Picture image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.g = {0, 255, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.b = {0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    imgsoa::Pixel result{};
    bilinearInterpolate(image, 0.5, 0.5, result);// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    EXPECT_EQ(result.red, 127);
    EXPECT_EQ(result.green, 127);
    EXPECT_EQ(result.blue, 127);
}

// Prueba la interpolación en el borde inferior derecho de la imagen (1.0, 1.0)
TEST(ImageSOATest, BilinearInterpolateFunctionEdge) {
    imgsoa::Picture image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.g = {0, 255, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.b = {0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    imgsoa::Pixel result{};
    bilinearInterpolate(image, 1.0, 1.0, result);

    EXPECT_EQ(result.red, 255);
    EXPECT_EQ(result.green, 255);
    EXPECT_EQ(result.blue, 255);
}

// Prueba la interpolación con coordenadas fuera del rango
TEST(ImageSOATest, BilinearInterpolateCoordinatesOutOfRange) {
    imgsoa::Picture image;
    image.width = 2;
    image.height = 2;
    image.r = {255, 0, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.g = {0, 255, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.b = {0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    imgsoa::Pixel result{};
    bilinearInterpolate(image, 2.0, 2.0, result);// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    EXPECT_EQ(result.red, 255);
    EXPECT_EQ(result.green, 255);
    EXPECT_EQ(result.blue, 255);
}

// Prueba la interpolación en una imagen de un solo píxel
TEST(ImageSOATest, BilinearInterpolateSinglePixelImage) {
    imgsoa::Picture image;
    image.width = 1;
    image.height = 1;
    image.r = {128};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.g = {128};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.b = {128};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    imgsoa::Pixel result{};
    bilinearInterpolate(image, 0.0, 0.0, result);

    EXPECT_EQ(result.red, 128);
    EXPECT_EQ(result.green, 128);
    EXPECT_EQ(result.blue, 128);
}

// Interpolación en los bordes
TEST(ImageSOATest, BilinearInterpolateAtPixelEdges) {
    imgsoa::Picture image;
    image.width = 2;
    image.height = 2;
    image.r = {100, 200, 150, 250};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.g = {100, 200, 150, 250};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    image.b = {100, 200, 150, 250};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    imgsoa::Pixel result{};
    bilinearInterpolate(image, 1.0, 0.0, result);

    EXPECT_EQ(result.red, 200);
    EXPECT_EQ(result.green, 200);
    EXPECT_EQ(result.blue, 200);
}

// Tests para la función resizeImage
// Prueba de redimensionamiento a un tamaño más grande
TEST(ImageResizeTest, ResizeToLargerSizeSOA) {
    imgsoa::Picture originalImage;
    originalImage.width = 2;
    originalImage.height = 2;
    originalImage.r = {255, 0, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    originalImage.g = {0, 255, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    originalImage.b = {0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    int const newWidth = 4;
    int const newHeight = 4;

    // Ejecutar la función de redimensionamiento
    imgsoa::Picture resizedImage = resizeImage(originalImage, newWidth, newHeight);

    // Verificar el tamaño de la imagen redimensionada
    EXPECT_EQ(resizedImage.width, newWidth);
    EXPECT_EQ(resizedImage.height, newHeight);
    EXPECT_EQ(resizedImage.r.size(), static_cast<size_t>(newWidth * newHeight));

    // Opcional: Verificar valores específicos de píxeles si se desea
    // Verificar que el píxel en la esquina superior izquierda sigue siendo rojo
    EXPECT_NEAR(resizedImage.r[0], 255, 1);
    EXPECT_NEAR(resizedImage.g[0], 0, 1);
    EXPECT_NEAR(resizedImage.b[0], 0, 1);
}

// Prueba de redimensionamiento a un tamaño más pequeño
TEST(ImageResizeTest, ResizeToSmallerSizeSOA) {
    imgsoa::Picture originalImage;
    originalImage.width = 4;
    originalImage.height = 4;
    // Crear una imagen con patrones de colores
    originalImage.r = {255, 255, 0, 0,// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                       255, 255, 0, 0,// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                       0, 0, 255, 255,// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                       0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    originalImage.g = {0, 0, 255, 255,// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                       0, 0, 255, 255,// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                       0, 0, 255, 255,// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                       0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    originalImage.b = {0, 0, 0, 0,
                       0, 0, 0, 0,
                       255, 255, 255, 255,// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                       255, 255, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    int const newWidth = 2;
    int const newHeight = 2;

    // Ejecutar la función de redimensionamiento
    imgsoa::Picture resizedImage = resizeImage(originalImage, newWidth, newHeight);

    // Verificar el tamaño de la imagen redimensionada
    EXPECT_EQ(resizedImage.width, newWidth);
    EXPECT_EQ(resizedImage.height, newHeight);
    EXPECT_EQ(resizedImage.r.size(), static_cast<size_t>(newWidth * newHeight));

    // Verificar valores específicos de píxeles
    EXPECT_NEAR(resizedImage.r[0], 255, 1);  // Debe ser aproximadamente rojo
    EXPECT_NEAR(resizedImage.g[0], 0, 1);
    EXPECT_NEAR(resizedImage.b[0], 0, 1);
}

// Prueba de redimensionamiento a dimensiones no uniformes
TEST(ImageResizeTest, ResizeNonUniformDimensionsSOA) {
    imgsoa::Picture originalImage;
    originalImage.width = 3;
    originalImage.height = 2;
    originalImage.r = {255, 0, 255, 0, 255, 0};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    originalImage.g = {0, 255, 0, 255, 0, 255}; // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    originalImage.b = {0, 0, 255, 255, 0, 0}; // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    int const newWidth = 5;
    int const newHeight = 3;

    // Ejecutar la función de redimensionamiento
    imgsoa::Picture const resizedImage = resizeImage(originalImage, newWidth, newHeight);

    // Verificar el tamaño de la imagen redimensionada
    EXPECT_EQ(resizedImage.width, newWidth);
    EXPECT_EQ(resizedImage.height, newHeight);
    EXPECT_EQ(resizedImage.r.size(), static_cast<size_t>(newWidth * newHeight));
}

// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

int main(int argc, char **argv) {
::testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}
