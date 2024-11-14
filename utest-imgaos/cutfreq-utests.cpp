#include <gtest/gtest.h>
#include "imgaos/imageaos.hpp"
#include <unordered_map>
#include <vector>

// Definir GRID_SIZE
static constexpr int GRID_SIZE = 8;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)

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

// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}