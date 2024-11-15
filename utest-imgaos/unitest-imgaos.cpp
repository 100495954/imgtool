//
// Created by pablo on 15/11/24.
//


#include "imgaos/imageaos.hpp"
#include "common/progargs.hpp"

#include <gtest/gtest.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <fstream>


static constexpr int GRID_SIZE = 8;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)

// Test del constructor de Image con diferentes tamaños y valores de color máximo
TEST(ImageTest, ConstructorDifferentSizesAndMaxColor) {
    // Tamaño pequeño
    imgaos::image_size const size_small = {.width = 10, .height = 10};
    imgaos::Image const img_small(size_small, 255);
    EXPECT_EQ(img_small.pixels.size(), 100);

    // Tamaño grande
    imgaos::image_size const size_large = {.width = 500, .height=500};
    imgaos::Image const img_large(size_large, 65535);
    EXPECT_EQ(img_large.pixels.size(), 250000);
    EXPECT_EQ(img_large.max_color, 65535);
}

// Test para comprobar que getColors cuenta colores únicos
TEST(GetColorsTest, UniqueColors) {
    int const max_rgb = 255;
    int const half_rgb = 128;
    int const quarter_rgb = 64;
    int const min_rgb = 32;
    imgaos::image_size const size = {.width = 3, .height=3};
    imgaos::Image img(size, max_rgb);

    // Colores únicos
    img.pixels = {
        {.r=max_rgb, .g=0, .b=0}, {.r=0, .g=max_rgb, .b=0}, {.r=0, .g=0, .b=max_rgb},
        {.r=max_rgb, .g=max_rgb, .b=0}, {.r=0, .g=max_rgb, .b=max_rgb}, {.r=max_rgb, .g=0, .b=max_rgb},
        {.r=half_rgb, .g=half_rgb, .b=half_rgb}, {.r=quarter_rgb, .g=quarter_rgb, .b=quarter_rgb}, {.r=min_rgb, .g=min_rgb, .b=min_rgb}
    };

    std::map<imgaos::Pixel, uint32_t> const color_map = getColors(img.pixels, img);
    EXPECT_EQ(color_map.size(), 9);  // Todos son colores únicos
}

// Test para getColors con colores repetidos
TEST(GetColorsTest, RepeatedColors) {
    int const max_rgb = 255;
    imgaos::image_size const size = {.width = 3, .height=3};
    imgaos::Image img(size, max_rgb);

    img.pixels = {
        {.r=max_rgb, .g=0, .b=0}, {.r=max_rgb, .g=0, .b=0}, {.r=max_rgb, .g=0, .b=0},
        {.r=0, .g=max_rgb, .b=0}, {.r=0, .g=max_rgb, .b=0}, {.r=0, .g=max_rgb, .b=0},
        {.r=0, .g=0, .b=max_rgb}, {.r=0, .g=0, .b=max_rgb}, {.r=0, .g=0, .b=max_rgb}
    };

    std::map<imgaos::Pixel, uint32_t> const color_map = getColors(img.pixels, img);
    EXPECT_EQ(color_map.size(), 3);  // Solo hay 3 colores únicos
}

// Test para verificar que los índices se asignan correctamente
TEST(GetColorsTest, ColorIndices) {
    int const max_rgb = 255;
    int const ind5 = 5;
    int const ind6 = 6;
    int const ind7 = 7;
    int const ind8 =8;
    imgaos::image_size const size = {.width=3, .height=3};
    imgaos::Image img(size, max_rgb);

    img.pixels[0] = {.r=max_rgb, .g=0, .b=0};
    img.pixels[1] = {.r=0, .g=max_rgb, .b=0};
    img.pixels[2] = {.r=0, .g=0, .b=max_rgb};
    img.pixels[3] = {.r=max_rgb, .g=0, .b=0};
    img.pixels[4] = {.r=0, .g=max_rgb, .b=0};
    img.pixels[ind5] = {.r=max_rgb, .g=max_rgb, .b=0};
    img.pixels[ind6] = {.r=max_rgb, .g=0, .b=0};
    img.pixels[ind7] = {.r=max_rgb, .g=max_rgb, .b=max_rgb};
    img.pixels[ind8] = {.r=0, .g=0, .b=0};

    imgaos::Pixel const pix1 = {.r=max_rgb, .g=0, .b=0};
    imgaos::Pixel const pix2 = {.r=0, .g=max_rgb, .b=0};
    imgaos::Pixel const pix3 = {.r=max_rgb, .g=max_rgb, .b=0};

    std::map<imgaos::Pixel, uint32_t> color_map = getColors(img.pixels, img);
  //AL USAR UNA ESTRUCTURA MAP QUE ORDENA LOS PIXELES DEBEMOS PONER EL ORDEN SEGUN R G B Y EN ESE ORDEN
  // 0:{0,0,0}, 1:{0,0,255}, 2:{0,255,0}, 3:{255,0,0}, 4:{255,255,0}, 5:{255,255,255}
    EXPECT_EQ(color_map[pix1], 3);
    EXPECT_EQ(color_map[pix2], 2);
    EXPECT_EQ(color_map[pix3], 4);
}

// Test para write_dataCPPM con 8 bits de profundidad de color
TEST(WriteDataCPPMTest, EightBitColorDepth) {
    int const max_rgb = 255;
    imgaos::image_size const size = {.width=2, .height=2};
    imgaos::Image img(size, max_rgb);
    img.pixels = {
        {.r=max_rgb, .g=0, .b=0}, {.r=0, .g=max_rgb, .b=0},
        {.r=0, .g=0, .b=max_rgb}, {.r=max_rgb, .g=max_rgb, .b=0}
    };

    std::map<imgaos::Pixel, uint32_t> color_map = getColors(img.pixels, img);
    std::string const filename = "test_output.cppm";
    write_dataCPPM(img, color_map, filename);

    std::ifstream file(filename, std::ios::binary);
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line);
    EXPECT_EQ(line, "C6 2 2 255 4");

    file.close();
    (void)std::remove(filename.c_str());
}

// Test para write_dataCPPM con 16 bits de profundidad de color
TEST(WriteDataCPPMTest, SixteenBitColorDepth) {
    int const max_rgb = 65535;
    imgaos::image_size const size = {.width=2, .height=2};
    imgaos::Image img(size, max_rgb);
    img.pixels = {
        {.r=max_rgb, .g=0, .b=0}, {.r=0, .g=max_rgb, .b=0},
        {.r=0, .g=0, .b=max_rgb}, {.r=max_rgb, .g=max_rgb, .b=0}
    };

    std::map<imgaos::Pixel, uint32_t> color_map = getColors(img.pixels, img);
    std::string const filename = "test_output_16.cppm";
    write_dataCPPM(img, color_map, filename);

    std::ifstream file(filename, std::ios::binary);
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line);
    EXPECT_EQ(line, "C6 2 2 65535 4");

    file.close();
    (void)std::remove(filename.c_str());
}

// Test para compress con archivo PPM válido
TEST(CompressTest, ValidPPMFile) {
    int const max_rgb = 255;
    std::ofstream file("test_valid.ppm", std::ios::binary);
    file << "P6 2 2 255\n";
    file << static_cast<char>(max_rgb) << static_cast<char>(0) << static_cast<char>(0);
    file << static_cast<char>(0) << static_cast<char>(max_rgb) << static_cast<char>(0);
    file << static_cast<char>(0) << static_cast<char>(0) << static_cast<char>(max_rgb);
    file << static_cast<char>(max_rgb) << static_cast<char>(max_rgb) << static_cast<char>(0);
    file.close();

    progargsCommon::parameters_files params = {.input_file="test_valid.ppm", .output_file="test_output.cppm"};
    imgaos::compress(params);

    std::ifstream output_file("test_output.cppm", std::ios::binary);
    ASSERT_TRUE(output_file.is_open());

    std::string line;
    std::getline(output_file, line);
    EXPECT_EQ(line, "C6 2 2 255 4");

    output_file.close();
    (void)std::remove("test_valid.ppm");
    (void)std::remove("test_output.cppm");
}

// Test para compress con archivo PPM no válido
TEST(CompressTest, InvalidPPMFileFormat) {
  // Crear archivo de prueba con encabezado incorrecto (P3 en lugar de P6)
  std::ofstream file("test_invalid.ppm", std::ios::binary);
  file << "P3 2 2 255\n";
  file.close();

  progargsCommon::parameters_files params = {.input_file="test_invalid.ppm", .output_file="test_output.cppm"};

  // Capturar la salida de error estándar
  testing::internal::CaptureStderr();
  imgaos::compress(params);
  std::string const output = testing::internal::GetCapturedStderr();

  // Verificar que el mensaje de error contenga la advertencia esperada
  EXPECT_TRUE(output.find("Invalid file type, the file:test_invalid.ppm must be in PPM format") != std::string::npos);

  // Eliminar archivo de prueba
  (void)std::remove("test_invalid.ppm");
}

TEST(ImageAOSTest, ReadPPMFunctionFileNotFound) {
    std::size_t width = 0;
    std::size_t height = 0;
    ASSERT_THROW(imgaos::readPPM("nonexistent.ppm", width, height), std::runtime_error);
}

// Test that readPPM fails if the file has an invalid format
TEST(ImageAOSTest, ReadPPMFunctionInvalidFormat) {
    std::ofstream file("invalid.ppm");
    file << "P3\n";
    file.close();
    std::size_t width = 0;
    std::size_t height = 0;
    ASSERT_THROW(imgaos::readPPM("invalid.ppm", width, height), std::runtime_error);
}

// Tests for writePPM

// Test that writePPM successfully writes the image and the file can be opened afterward
TEST(ImageAOSTest, WritePPMFunctionSuccess) {
    std::vector<imgaos::Pixel> const image = {
            {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0},
            {.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}
    };
    std::size_t const width = 2;
    std::size_t const height = 2;
    imgaos::writePPM("output.ppm", image, width, height);
    std::ifstream file("output.ppm", std::ios::binary);
    ASSERT_TRUE(file.is_open());
}

// Test that writePPM fails when trying to write to an invalid path
TEST(ImageAOSTest, WritePPMFunctionFileNotWritable) {
    std::vector<imgaos::Pixel> const image = {
            {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0},
            {.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}
    };
    std::size_t const width = 2;
    std::size_t const height = 2;
    ASSERT_THROW(imgaos::writePPM("/invalid_path/output.ppm", image, width, height), std::runtime_error);
}

// Test para countColorFrequencyWithParams - Caso 1: Imagen con colores repetidos
TEST(ImageAOSTest, CountColorFrequencyWithParams_RepeatedColors) {
    imgaos::Pixel const red = {.r=255, .g=0, .b=0};
    imgaos::Pixel const green = {.r=0, .g=255, .b=0};
    imgaos::Pixel const blue = {.r=0, .g=0, .b=255};

    std::vector<imgaos::Pixel> const image1 = {red, red, green, blue};
    imgaos::CutFreqParams const params1(image1, {.width=2, .height=2}, 1);
    auto frequencyMap1 = imgaos::countColorFrequencyWithParams(params1);

    EXPECT_EQ(frequencyMap1[red], 2);
    EXPECT_EQ(frequencyMap1[green], 1);
    EXPECT_EQ(frequencyMap1[blue], 1);
}

// Test par countColorFrequencyWithParams - Caso 2: Imagen sin colores repetidos
TEST(ImageAOSTest, CountColorFrequencyWithParams_UniqueColors) {
    imgaos::Pixel const red = {.r=255, .g=0, .b=0};
    imgaos::Pixel const green = {.r=0, .g=255, .b=0};
    imgaos::Pixel const blue = {.r=0, .g=0, .b=255};
    imgaos::Pixel const white = {.r=255, .g=255, .b=255};

    std::vector<imgaos::Pixel> const image2 = {red, green, blue, white};
    imgaos::CutFreqParams const params2(image2, {.width=2, .height=2}, 1);
    auto frequencyMap2 = imgaos::countColorFrequencyWithParams(params2);

    EXPECT_EQ(frequencyMap2[red], 1);
    EXPECT_EQ(frequencyMap2[green], 1);
    EXPECT_EQ(frequencyMap2[blue], 1);
    EXPECT_EQ(frequencyMap2[white], 1);
}

// Test para countColorFrequencyWithParams - Caso 3: Imagen con un solo color
TEST(ImageAOSTest, CountColorFrequencyWithParams_SingleColor) {
    imgaos::Pixel const red = {.r=255, .g=0, .b=0};

    std::vector<imgaos::Pixel> const image = {red, red, red, red};
    imgaos::CutFreqParams const params(image, {.width=2, .height=2}, 1);
    auto frequencyMap = imgaos::countColorFrequencyWithParams(params);

    EXPECT_EQ(frequencyMap[red], 4);
}

// Test para getLeastFrequentColors - Caso 1: Un color menos frecuente
TEST(ImageAOSTest, GetLeastFrequentColors_OneLeastFrequent) {
    std::unordered_map<imgaos::Pixel, int, imgaos::PixelHash> const frequencyMap1 = {
            {{.r=255, .g=0, .b=0}, 2}, {{.r=0, .g=255, .b=0}, 1}, {{.r=0, .g=0, .b=255}, 1}
    };
    auto colorsToRemove1 = imgaos::getLeastFrequentColors(frequencyMap1, 1);

    EXPECT_EQ(colorsToRemove1.size(), 1);
    EXPECT_EQ(colorsToRemove1[0].r, 0);
    EXPECT_EQ(colorsToRemove1[0].g, 0);
    EXPECT_EQ(colorsToRemove1[0].b, 255);
}

// Test para getLeastFrequentColors - Caso 2: Dos colores menos frecuentes
TEST(ImageAOSTest, GetLeastFrequentColors_TwoLeastFrequent) {
    std::unordered_map<imgaos::Pixel, int, imgaos::PixelHash> const frequencyMap1 = {
            {{.r=255, .g=0, .b=0}, 2}, {{.r=0, .g=255, .b=0}, 1}, {{.r=0, .g=0, .b=255}, 1}
    };
    auto colorsToRemove2 = imgaos::getLeastFrequentColors(frequencyMap1, 2);

    EXPECT_EQ(colorsToRemove2.size(), 2);
    EXPECT_TRUE((colorsToRemove2[0].r == 0 && colorsToRemove2[0].g == 0 && colorsToRemove2[0].b == 255) ||
                (colorsToRemove2[1].r == 0 && colorsToRemove2[1].g == 0 && colorsToRemove2[1].b == 255));
}

// Test para getLeastFrequentColors - Caso 3: Todos los colores con la misma frecuencia
TEST(ImageAOSTest, GetLeastFrequentColors_SameFrequency) {
    std::unordered_map<imgaos::Pixel, int, imgaos::PixelHash> const frequencyMap = {
            {{.r=255, .g=0, .b=0}, 1}, {{.r=0, .g=255, .b=0}, 1}, {{.r=0, .g=0, .b=255}, 1}
    };
    auto colorsToRemove = imgaos::getLeastFrequentColors(frequencyMap, 2);

    EXPECT_EQ(colorsToRemove.size(), 2);
}

// Test para buildGrid - Caso 1: Imagen con colores repetidos
TEST(ImageAOSTest, BuildGrid_RepeatedColors) {
    std::vector<imgaos::Pixel> const image1 = {
            {.r=255, .g=0, .b=0}, {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=0, .b=255}
    };
    auto grid1 = imgaos::buildGrid(image1, 8);

    EXPECT_EQ(grid1.size(), 3);
}

// Test para buildGrid - Caso 2: Imagen sin colores repetidos
TEST(ImageAOSTest, BuildGrid_UniqueColors) {
    std::vector<imgaos::Pixel> const image2 = {
            {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}
    };
    auto grid2 = imgaos::buildGrid(image2, 8);

    EXPECT_EQ(grid2.size(), 4);
}

// Test para buildGrid - Caso 3: Imagen con colores en los límites del grid
TEST(ImageAOSTest, BuildGrid_BoundaryColors) {
    std::vector<imgaos::Pixel> const image = {
            {.r=0, .g=0, .b=0}, {.r=255, .g=255, .b=255}
    };
    auto grid = imgaos::buildGrid(image, 8);

    EXPECT_EQ(grid.size(), 2);
}

// Test para findClosestColorOptimized - Caso 1: Color objetivo cercano a un color en la imagen
TEST(ImageAOSTest, FindClosestColorOptimized_NearColor) {
    std::vector<imgaos::Pixel> const image1 = {
            {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=0, .b=255}, {.r=128, .g=128, .b=128}
    };
    auto grid1 = imgaos::buildGrid(image1, GRID_SIZE);

    imgaos::Pixel const targetColor1 = {.r=127, .g=127, .b=127};
    imgaos::Pixel const closestColor1 = imgaos::findClosestColorOptimized(targetColor1, grid1, GRID_SIZE);

    EXPECT_EQ(closestColor1.r, 128);
    EXPECT_EQ(closestColor1.g, 128);
    EXPECT_EQ(closestColor1.b, 128);
}

// Test para findClosestColorOptimized - Caso 2: Color objetivo igual a un color en la imagen
TEST(ImageAOSTest, FindClosestColorOptimized_ExactColor) {
    std::vector<imgaos::Pixel> const image1 = {
            {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=0, .b=255}, {.r=128, .g=128, .b=128}
    };
    auto grid1 = imgaos::buildGrid(image1, GRID_SIZE);

    imgaos::Pixel const targetColor2 = {.r=255, .g=0, .b=0};
    imgaos::Pixel const closestColor2 = imgaos::findClosestColorOptimized(targetColor2, grid1, GRID_SIZE);

    EXPECT_EQ(closestColor2.r, 255);
    EXPECT_EQ(closestColor2.g, 0);
    EXPECT_EQ(closestColor2.b, 0);
}

// Test para findClosestColorOptimized - Caso 3: Color objetivo fuera del rango de la imagen
TEST(ImageAOSTest, FindClosestColorOptimized_OutOfRange) {
    std::vector<imgaos::Pixel> const image = {
            {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=0, .b=255}
    };
    auto grid = imgaos::buildGrid(image, GRID_SIZE);

    imgaos::Pixel const targetColor = {.r=300, .g=300, .b=300};
    imgaos::Pixel const closestColor = imgaos::findClosestColorOptimized(targetColor, grid, GRID_SIZE);

    EXPECT_EQ(closestColor.r, 0);
    EXPECT_EQ(closestColor.g, 0);
    EXPECT_EQ(closestColor.b, 0);
}

TEST(PhotoTest, LoadFunctionFileNotFound) {
    imgaos::Photo photo;
    // Intenta cargar un archivo que no existe y verifica que la función falle sin causar un fallo de segmentación
    ASSERT_FALSE(imgaos::loadPhoto(photo, "nonexistent.ppm"));
}

// Prueba que load falle si el archivo tiene un formato incorrecto
TEST(PhotoTest, LoadFunctionInvalidFormat) {
    std::ofstream file("invalid.ppm");
    file << "P3\n";
    file.close();
    imgaos::Photo photo;
    // Intenta cargar un archivo con un formato incorrecto y verifica que la función falle sin causar un fallo de segmentación
    ASSERT_FALSE(imgaos::loadPhoto(photo, "invalid.ppm"));
}

// Tests para save

// Verifica que save guarde correctamente la imagen y que el archivo pueda abrirse después
TEST(PhotoTest, SaveFunctionSuccess) {
    imgaos::Photo photo;
    imgaos::loadPhoto(photo, "test_image.ppm");
    ASSERT_TRUE(imgaos::savePhoto(photo, "output_image.ppm"));
    std::ifstream file("output_image.ppm", std::ios::binary);
    ASSERT_TRUE(file.is_open());
}

// Test para verificar que save falle al intentar guardar en una ruta inválida
TEST(PhotoTest, SaveFunctionFileNotWritable) {
    imgaos::Photo photo;
    imgaos::loadPhoto(photo, "test_image.ppm");
    // Intenta guardar en una ubicación inválida y verifica que falle sin causar un fallo de segmentación
    ASSERT_FALSE(imgaos::savePhoto(photo, "/invalid_path/output_image.ppm"));
}

// Caso 1: de 255 a 128
TEST(MaxLevelTest, MaxLevelScaling_255to128) {
    imgaos::Photo photo;
    photo.width = 2;
    photo.height = 2;
    photo.maxColorValue = 255;
    photo.pixels = {
        {.r = 0, .g = 128, .b = 255},
        {.r = 255, .g = 255, .b = 255},
        {.r = 128, .g = 128, .b = 128},
        {.r = 64, .g = 64, .b = 64}
    };

    imgaos::maxlevel(photo, 128);

    EXPECT_EQ(photo.pixels[0].r, 0);
    EXPECT_EQ(photo.pixels[0].g, 64);
    EXPECT_EQ(photo.pixels[0].b, 128);
    EXPECT_EQ(photo.pixels[1].r, 128);
    EXPECT_EQ(photo.pixels[1].g, 128);
    EXPECT_EQ(photo.pixels[1].b, 128);
    EXPECT_EQ(photo.pixels[2].r, 64);
    EXPECT_EQ(photo.pixels[2].g, 64);
    EXPECT_EQ(photo.pixels[2].b, 64);
    EXPECT_EQ(photo.pixels[3].r, 32);
    EXPECT_EQ(photo.pixels[3].g, 32);
    EXPECT_EQ(photo.pixels[3].b, 32);
    EXPECT_EQ(photo.maxColorValue, 128);
}

// Caso 2: de 255 a 255 (mismo valor)
TEST(MaxLevelTest, MaxLevelScaling_SameMaxValue) {
    imgaos::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 255;
    photo.pixels = {{.r = 0, .g = 128, .b = 255}};

    imgaos::maxlevel(photo, 255);

    EXPECT_EQ(photo.pixels[0].r, 0);
    EXPECT_EQ(photo.pixels[0].g, 128);
    EXPECT_EQ(photo.pixels[0].b, 255);
    EXPECT_EQ(photo.maxColorValue, 255);
}

// Caso 3: de 255 a 0
TEST(MaxLevelTest, MaxLevelScaling_MaxValueZero) {
    imgaos::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 255;
    photo.pixels = {{.r = 0, .g = 128, .b = 255}};

    imgaos::maxlevel(photo, 0);

    EXPECT_EQ(photo.pixels[0].r, 0);
    EXPECT_EQ(photo.pixels[0].g, 0);
    EXPECT_EQ(photo.pixels[0].b, 0);
    EXPECT_EQ(photo.maxColorValue, 0);
}

// Caso 4: de 128 a 255
TEST(MaxLevelTest, MaxLevelScaling_128to255) {
    imgaos::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 128;
    photo.pixels = {{.r = 0, .g = 64, .b = 128}};

    imgaos::maxlevel(photo, 255);

    EXPECT_EQ(photo.pixels[0].r, 0);
    EXPECT_EQ(photo.pixels[0].g, 127);
    EXPECT_EQ(photo.pixels[0].b, 255);
    EXPECT_EQ(photo.maxColorValue, 255);
}

// Caso 5: de 255 a 65535
TEST(MaxLevelTest, ScaleTo65535) {
    imgaos::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 255;
    photo.pixels = {{.r=0, .g=128, .b=255}};

    imgaos::maxlevel(photo, 65535);

    EXPECT_EQ(photo.pixels[0].r, 0);
    EXPECT_EQ(photo.pixels[0].g, 32896);
    EXPECT_EQ(photo.pixels[0].b, 65535);
    EXPECT_EQ(photo.maxColorValue, 65535);
}

// Caso 6: de 65535 a 255
TEST(MaxLevelTest, ScaleFrom65535to255) {
    imgaos::Photo photo;
    photo.width = 1;
    photo.height = 1;
    photo.maxColorValue = 65535;
    photo.pixels = {{.r=0, .g=32896, .b=65535}};

    imgaos::maxlevel(photo, 255);

    EXPECT_EQ(photo.pixels[0].r, 0);
    EXPECT_EQ(photo.pixels[0].g, 128);
    EXPECT_EQ(photo.pixels[0].b, 255);
    EXPECT_EQ(photo.maxColorValue, 255);
}

TEST(ImageAOSTest, LoadPPMFunctionFileNotFound) {
    std::vector<std::vector<imgaos::Pixel>> image;
    // Intenta cargar un archivo que no existe y verifica que la función falle sin causar un fallo de segmentación
    ASSERT_FALSE(imgaos::loadPPM("nonexistent.ppm", image));
}
// Prueba que loadPPM falle si el archivo tiene un formato incorrecto
TEST(ImageAOSTest, LoadPPMFunctionInvalidFormat) {
    std::ofstream file("invalid.ppm");
    file << "P3\n";
    file.close();
    std::vector<std::vector<imgaos::Pixel>> image;
    // Intenta cargar un archivo con un formato incorrecto y verifica que la función falle sin causar un fallo de segmentación
    ASSERT_FALSE(imgaos::loadPPM("invalid.ppm", image));
}


// Tests para la función savePPM
// Verifica que savePPM guarde correctamente la imagen y que el archivo pueda abrirse después
TEST(ImageAOSTest, SavePPMFunctionSuccess) {
    std::vector<std::vector<imgaos::Pixel>> const image = {
            {{.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}},
            {{.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}}
    };
    ASSERT_TRUE(imgaos::savePPM("output.ppm", image));
    std::ifstream file("output.ppm", std::ios::binary);
    ASSERT_TRUE(file.is_open());
}

// Test para verificar que `savePPM` falle al intentar guardar en una ruta inválida
TEST(ImageAOSTest, SavePPMFunctionFileNotWritable) {
    std::vector<std::vector<imgaos::Pixel>> const image = {
            {{.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}},
            {{.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}}
    };
    // Intenta guardar en una ubicación inválida y verifica que falle sin causar un fallo de segmentación
    ASSERT_FALSE(imgaos::savePPM("/invalid_path/output.ppm", image));
}

// Tests para la función bilinearInterpolate
// Prueba la interpolación en el centro de una imagen 2x2
TEST(ImageAOSTest, BilinearInterpolateFunctionCenter) {
    std::vector<std::vector<imgaos::Pixel>> const image = {
            {{.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}},
            {{.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}}
    };
    imgaos::Pixel const result = imgaos::bilinearInterpolate(image, 0.5, 0.5);
    EXPECT_EQ(result.r, 127);
    EXPECT_EQ(result.g, 127);
    EXPECT_EQ(result.b, 127);
}

// Prueba la interpolación en el borde inferior derecho de la imagen (1.0, 1.0)
TEST(ImageAOSTest, BilinearInterpolateFunctionEdge) {
    std::vector<std::vector<imgaos::Pixel>> const image = {
            {{.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}},
            {{.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}}
    };
    imgaos::Pixel const result = imgaos::bilinearInterpolate(image, 1.0, 1.0);
    EXPECT_EQ(result.r, 255);
    EXPECT_EQ(result.g, 255);
    EXPECT_EQ(result.b, 255);
}

// coordenadas fuera de rango
TEST(ImageAOSTest, BilinearInterpolateNegativeCoordinates) {
    std::vector<std::vector<imgaos::Pixel>> const image = {
            {{.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}},
            {{.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}}
    };
    imgaos::Pixel const result = imgaos::bilinearInterpolate(image, -0.5, -0.5);
    EXPECT_EQ(result.r, 255);
    EXPECT_EQ(result.g, 0);
    EXPECT_EQ(result.b, 0);
}

// interpolación en imagen de un solo píxel
TEST(ImageAOSTest, BilinearInterpolateSinglePixelImage) {
    std::vector<std::vector<imgaos::Pixel>> const image = {{{.r=100, .g=100, .b=100}}};
    imgaos::Pixel const result = imgaos::bilinearInterpolate(image, 0.5, 0.5);
    EXPECT_EQ(result.r, 100);
    EXPECT_EQ(result.g, 100);
    EXPECT_EQ(result.b, 100);
}

// Interpolación en los bordes
TEST(ImageAOSTest, BilinearInterpolateAtPixelEdges) {
    std::vector<std::vector<imgaos::Pixel>> const image = {
            {{.r=100, .g=100, .b=100}, {.r=200, .g=200, .b=200}},
            {{.r=150, .g=150, .b=150}, {.r=250, .g=250, .b=250}}
    };
    imgaos::Pixel const result = imgaos::bilinearInterpolate(image, 1.0, 0.0);
    EXPECT_EQ(result.r, 200);
    EXPECT_EQ(result.g, 200);
    EXPECT_EQ(result.b, 200);
}


using imgaos::Pixel;
using std::vector;
// Prueba de redimensionamiento a un tamaño más grande
TEST(ImageResizeTest, ResizeToLargerSize) {
    // Imagen original de 2x2 píxeles
    vector<vector<Pixel>> const originalImage = {
            { {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0} },
            { {.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=0} }
    };

    int const newWidth = 4;
    int const newHeight = 4;

    // Ejecutar la función de redimensionamiento
    auto resizedImage = imgaos::resizeImage(originalImage, newWidth, newHeight);

    // Verificar el tamaño de la imagen redimensionada
    EXPECT_EQ(resizedImage.size(), static_cast<size_t>(newHeight));
    EXPECT_EQ(resizedImage[0].size(), static_cast<size_t>(newWidth));

    // Opcional: Verificar valores específicos de píxeles si se desea
    // Por ejemplo, verificar que el píxel en la esquina superior izquierda sigue siendo rojo
    EXPECT_NEAR(resizedImage[0][0].r, 255, 1);
    EXPECT_NEAR(resizedImage[0][0].g, 0, 1);
    EXPECT_NEAR(resizedImage[0][0].b, 0, 1);
}

// Prueba de redimensionamiento a un tamaño más pequeño
TEST(ImageResizeTest, ResizeToSmallerSize) {
    // Imagen original de 4x4 píxeles
    vector<vector<Pixel>> const originalImage = {
            { {.r=255, .g=0, .b=0}, {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=255, .b=0} },
            { {.r=255, .g=0, .b=0}, {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=255, .b=0} },
            { {.r=0, .g=0, .b=255}, {.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=0}, {.r=255, .g=255, .b=0} },
            { {.r=0, .g=0, .b=255}, {.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=0}, {.r=255, .g=255, .b=0} }
    };

    int const newWidth = 2;
    int const newHeight = 2;

    // Ejecutar la función de redimensionamiento
    auto resizedImage = imgaos::resizeImage(originalImage, newWidth, newHeight);

    // Verificar el tamaño de la imagen redimensionada
    EXPECT_EQ(resizedImage.size(), static_cast<size_t>(newHeight));
    EXPECT_EQ(resizedImage[0].size(), static_cast<size_t>(newWidth));

    // Verificar valores específicos de píxeles
    EXPECT_NEAR(resizedImage[0][0].r, 255, 1);  // Debe ser aproximadamente rojo
    EXPECT_NEAR(resizedImage[0][0].g, 0, 1);
    EXPECT_NEAR(resizedImage[0][0].b, 0, 1);
}

// Prueba de redimensionamiento a dimensiones no uniformes
TEST(ImageResizeTest, ResizeToNonUniformSize) {
    // Imagen original de 4x4 píxeles
    vector<vector<Pixel>> const originalImage = {
            { {.r=255, .g=0, .b=0}, {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=255, .b=0} },
            { {.r=255, .g=0, .b=0}, {.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}, {.r=0, .g=255, .b=0} },
            { {.r=0, .g=0, .b=255}, {.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=0}, {.r=255, .g=255, .b=0} },
            { {.r=0, .g=0, .b=255}, {.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=0}, {.r=255, .g=255, .b=0} }
    };

    int const newWidth = 3;
    int const newHeight = 2;

    // Ejecutar la función de redimensionamiento
    auto resizedImage = imgaos::resizeImage(originalImage, newWidth, newHeight);

    // Verificar el tamaño de la imagen redimensionada
    EXPECT_EQ(resizedImage.size(), static_cast<size_t>(newHeight));
    EXPECT_EQ(resizedImage[0].size(), static_cast<size_t>(newWidth));

    // Verificar valores específicos de píxeles
    EXPECT_NEAR(resizedImage[0][0].r, 255, 1);  // Debe ser aproximadamente rojo
    EXPECT_NEAR(resizedImage[0][0].g, 0, 1);
    EXPECT_NEAR(resizedImage[0][0].b, 0, 1);
}
// Prueba de redimensionamiento a un tamaño muy grande
TEST(ImageResizeTest, ResizeToVeryLargeSize) {
    vector<vector<Pixel>> const originalImage = {
            {{.r=255, .g=0, .b=0}, {.r=0, .g=255, .b=0}},
            {{.r=0, .g=0, .b=255}, {.r=255, .g=255, .b=255}}
    };
    int const newWidth = 1000;
    int const newHeight = 1000;
    auto resizedImage = imgaos::resizeImage(originalImage, newWidth, newHeight);
    EXPECT_EQ(resizedImage.size(), static_cast<size_t>(newHeight));
    EXPECT_EQ(resizedImage[0].size(), static_cast<size_t>(newWidth));
}

// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

int main(int argc, char **argv) {
::testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}