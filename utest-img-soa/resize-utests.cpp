#include <gtest/gtest.h>
#include <fstream>
#include "imgsoa/imagesoa.hpp"



// Prueba que loadPPM falle si el archivo no existe
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

// Prueba de redimensionamiento a un tamaño muy grande
TEST(ImageResizeTest, ResizeToVeryLargeSizeSOA) {
    imgsoa::Picture originalImage;
    originalImage.width = 2;
    originalImage.height = 2;
    originalImage.r = {255, 0, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    originalImage.g = {0, 255, 0, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    originalImage.b = {0, 0, 255, 255};// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    int const newWidth = 1000;
    int const newHeight = 1000;

    // Ejecutar la función de redimensionamiento
    imgsoa::Picture const resizedImage = resizeImage(originalImage, newWidth, newHeight);

    // Verificar el tamaño de la imagen redimensionada
    EXPECT_EQ(resizedImage.width, newWidth);
    EXPECT_EQ(resizedImage.height, newHeight);
    EXPECT_EQ(resizedImage.r.size(), static_cast<size_t>(newWidth * newHeight));
}

// Ejecuta todos los tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
