#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

struct Pixel {
    uint8_t r, g, b;
};

struct CPPMFile {
    std::string magic_number;
    int width;
    int height;
    int max_color_value;
    int num_color_entries;
    std::vector<uint8_t> color_table;
    std::vector<uint8_t> pixel_data;
};

CPPMFile readCPPM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    CPPMFile cppm;

    // Read magic number
    file >> cppm.magic_number;
    if (cppm.magic_number != "C6") {
        throw std::runtime_error("Invalid magic number: " + cppm.magic_number);
    }

    // Read width, height, max color value, and number of color table entries
    file >> cppm.width >> cppm.height >> cppm.max_color_value >> cppm.num_color_entries;
    file.get();  // Consume the newline character after the header

    // Calculate color table size and read it
    int color_size = (cppm.max_color_value <= 255) ? 3 : 6;
    cppm.color_table.resize(cppm.num_color_entries * color_size);
    file.read(reinterpret_cast<char*>(cppm.color_table.data()), cppm.color_table.size());

    // Determine pixel size based on color entries
    int pixel_size;
    if (cppm.num_color_entries <= 256) {
        pixel_size = 1;
    } else if (cppm.num_color_entries <= 65536) {
        pixel_size = 2;
    } else {
        pixel_size = 4;
    }

    // Read pixel data
    size_t num_pixels = cppm.width * cppm.height;
    cppm.pixel_data.resize(num_pixels * pixel_size);
    file.read(reinterpret_cast<char*>(cppm.pixel_data.data()), cppm.pixel_data.size());

    return cppm;
}

bool compareCPPMFiles(const std::string& file1, const std::string& file2) {
    try {
        CPPMFile cppm1 = readCPPM(file1);
        CPPMFile cppm2 = readCPPM(file2);

        // Compare headers
        if (cppm1.magic_number != cppm2.magic_number ||
            cppm1.width != cppm2.width ||
            cppm1.height != cppm2.height ||
            cppm1.max_color_value != cppm2.max_color_value ||
            cppm1.num_color_entries != cppm2.num_color_entries) {
            return false;
        }

        // Compare color tables
        if (cppm1.color_table != cppm2.color_table) {
            return false;
        }

        // Compare pixel data
        if (cppm1.pixel_data != cppm2.pixel_data) {
            return false;
        }

        return true;  // Files are identical
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file1> <file2>\n";
        return 1;
    }

    std::string file1 = argv[1];
    std::string file2 = argv[2];

    if (compareCPPMFiles(file1, file2)) {
        std::cout << "The files are identical.\n";
    } else {
        std::cout << "The files are different.\n";
    }

    return 0;
}