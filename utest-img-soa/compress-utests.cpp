//
// Created by pablo on 15/11/24.
//

#include "common/progargs.hpp"
#include "imgsoa/imagesoa.hpp"

#include <common/binario.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <vector>

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
    std::string const filename = "";
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


