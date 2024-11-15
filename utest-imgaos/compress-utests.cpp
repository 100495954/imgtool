//
// Created by claud on 13/11/2024.
//


#include "imgaos/imageaos.hpp"
#include "common/progargs.hpp"

#include <gtest/gtest.h>
#include <map>
#include <vector>
#include <fstream>


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