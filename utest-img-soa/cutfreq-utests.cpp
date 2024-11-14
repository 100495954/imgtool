#include <gtest/gtest.h>
#include "imgsoa/imagesoa.hpp"
#include <unordered_map>

// Definir GRID_SIZE
static constexpr int GRID_SIZE = 8;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)

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

// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
