//
// Created by carme on 14/11/2024.
//

#include <gtest/gtest.h>
#include "imgsoa/imagesoa.hpp"
#include <vector>
#include <fstream>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)

// Tests for loadPhoto

// Test that loadPhoto fails if the file does not exist
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

// Tests for readHeader

// Test that readHeader fails if the file has an incorrect format
TEST(PhotoTest, ReadHeaderFunctionInvalidFormat) {
    std::ofstream file("invalid_header.ppm");
    file << "P3\n";
    file.close();
    std::ifstream inputFile("invalid_header.ppm", std::ios::binary);
    imgsoa::Photo photo;
    // Attempt to read a file with an incorrect format and verify that the function fails without causing a segmentation fault
    ASSERT_FALSE(imgsoa::readHeader(inputFile, photo));
}

// Tests for savePhoto

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

// Tests for maxlevel

// Verify that maxlevel correctly adjusts the pixel values
TEST(PhotoTest, MaxLevelFunctionSuccess) {
    imgsoa::Photo photo;
    imgsoa::loadPhoto("test_image.ppm", photo);
    imgsoa::maxlevel(photo, 128);
    EXPECT_EQ(photo.maxColorValue, 128);
    EXPECT_EQ(photo.red[0], 128);
    EXPECT_EQ(photo.green[0], 0);
    EXPECT_EQ(photo.blue[0], 0);
}

// Tests for handle_maxlevel_optionSOA

// Verify that handle_maxlevel_optionSOA correctly handles the maxlevel option
TEST(ImageSOATest, HandleMaxLevelOptionSOAFunctionSuccess) {
    std::vector<std::string> args = {"program", "test_image.ppm", "output_image.ppm", "maxlevel", "128"};
    progargsCommon::parameters_files params(args[1], args[2]);
    imgsoa::handle_maxlevel_optionSOA(args, params);
    imgsoa::Photo photo;
    imgsoa::loadPhoto("output_image.ppm", photo);
    EXPECT_EQ(photo.maxColorValue, 128);
    EXPECT_EQ(photo.red[0], 128);
    EXPECT_EQ(photo.green[0], 0);
    EXPECT_EQ(photo.blue[0], 0);
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

// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

//int main(int argc, char **argv) {
    //::testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();
//}