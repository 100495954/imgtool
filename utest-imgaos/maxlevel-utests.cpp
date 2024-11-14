#include <gtest/gtest.h>
#include "imgaos/imageaos.hpp"
#include <cstddef>
#include <vector>
#include <fstream>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)

// Tests para load

// Prueba que load falle si el archivo no existe
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

// Tests para readHeader

// Prueba que readHeader falle si el archivo tiene un formato incorrecto
TEST(PhotoTest, ReadHeaderFunctionInvalidFormat) {
    std::ofstream file("invalid_header.ppm");
    file << "P3\n";
    file.close();
    std::ifstream inputFile("invalid_header.ppm", std::ios::binary);
    imgaos::Photo photo;
    // Intenta leer un archivo con un formato incorrecto y verifica que la función falle sin causar un fallo de segmentación
    ASSERT_FALSE(imgaos::readHeader(inputFile, photo));
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

// Tests para maxlevel

// Verifica que maxlevel ajuste correctamente los valores de los píxeles
TEST(PhotoTest, MaxLevelFunctionSuccess) {
    imgaos::Photo photo;
    imgaos::loadPhoto(photo, "test_image.ppm");
    imgaos::maxlevel(photo, 128);
    EXPECT_EQ(photo.maxColorValue, 128);
    EXPECT_EQ(photo.pixels[0].r, 128);
    EXPECT_EQ(photo.pixels[0].g, 0);
    EXPECT_EQ(photo.pixels[0].b, 0);
}

// Tests para handle_maxlevel_optionAOS

// Verifica que handle_maxlevel_optionAOS maneje correctamente la opción maxlevel
TEST(ImageAOSTest, HandleMaxLevelOptionAOSFunctionSuccess) {
    std::vector<std::string> args = {"program", "test_image.ppm", "output_image.ppm", "maxlevel", "128"};
    progargsCommon::parameters_files params(args[1], args[2]);
    imgaos::handle_maxlevel_optionAOS(args, params);
    imgaos::Photo photo;
    imgaos::loadPhoto(photo, "output_image.ppm");
    EXPECT_EQ(photo.maxColorValue, 128);
    EXPECT_EQ(photo.pixels[0].r, 128);
    EXPECT_EQ(photo.pixels[0].g, 0);
    EXPECT_EQ(photo.pixels[0].b, 0);
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

// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}