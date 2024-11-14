#include <gtest/gtest.h>
#include <fstream>
#include "imgaos/imageaos.hpp"

// NOLINTBEGIN(cppcoreguidlines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)

/*
// Tests para la función clamp
// Verifica que clamp devuelva el valor dentro del rango si está en el rango
TEST(ImageAOSTest, ClampFunctionWithinRange) {
    EXPECT_EQ(imgaos::clamp(5, 0, 10), 5);
}

// Verifica que clamp limite correctamente el valor si está por debajo del rango
TEST(ImageAOSTest, ClampFunctionBelowRange) {
    EXPECT_EQ(imgaos::clamp(-1, 0, 10), 0);
}

// Verifica que clamp limite correctamente el valor si está por encima del rango
TEST(ImageAOSTest, ClampFunctionAboveRange) {
    EXPECT_EQ(imgaos::clamp(15, 0, 10), 10);
}

//Valor igual al límite inferior
TEST(ImageAOSTest, ClampFunctionAtMinValue) {
    EXPECT_EQ(imgaos::clamp(0, 0, 10), 0);
}

//Valor igual al límite superior
TEST(ImageAOSTest, ClampFunctionAtMaxValue) {
    EXPECT_EQ(imgaos::clamp(10, 0, 10), 10);
}

// Rango donde el mínimo es mayor que el máximo (caso inválido)
TEST(ImageAOSTest, ClampFunctionInvalidRange) {
    EXPECT_EQ(imgaos::clamp(5, 10, 0), 10);
}
*/

//Tests para loadPPM

// Prueba que loadPPM falle si el archivo no existe
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
//NOLINTEND(readability-magic-numbers)
//NOLINTEND(cppcoreguidlines-avoid-magic-numbers)


// Ejecuta todos los tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
