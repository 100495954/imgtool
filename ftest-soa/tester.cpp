#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

struct Pixel {
    uint8_t r, g, b;
};

bool loadPPM(const std::string &filename, std::vector<std::vector<Pixel>> &image, int &width, int &height) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: No ha sido posible abrir el archivo " << filename << "\n";
        return false;
    }

    std::string header;
    file >> header;
    if (header != "P6") {
        std::cerr << "Error: Formato PPM incorrecto (" << header << ")\n";
        return false;
    }

    file >> width >> height;
    int maxColorValue;
    file >> maxColorValue;
    file.ignore(256, '\n');

    if (maxColorValue != 255) {
        std::cerr << "Error: Unsupported max color value (" << maxColorValue << ")\n";
        return false;
    }

    image.resize(static_cast<std::vector<Pixel>::size_type>(height), std::vector<Pixel>(static_cast<std::vector<Pixel>::size_type>(width)));

    for (std::vector<Pixel>::size_type y = 0; y < static_cast<std::vector<Pixel>::size_type>(height); ++y) {
        for (std::vector<Pixel>::size_type x = 0; x < static_cast<std::vector<Pixel>::size_type>(width); ++x) {
            file.read(reinterpret_cast<char*>(&image[y][x].r), 1);
            file.read(reinterpret_cast<char*>(&image[y][x].g), 1);
            file.read(reinterpret_cast<char*>(&image[y][x].b), 1);
        }
    }
    return true;
}

void compareImages(const std::vector<std::vector<Pixel>> &image1, const std::vector<std::vector<Pixel>> &image2, int width, int height, int threshold = 5) {
    bool imagesAreEqual = true;  // Flag to track if any significant difference is found

    for (std::vector<Pixel>::size_type y = 0; y < static_cast<std::vector<Pixel>::size_type>(height); ++y) {
        for (std::vector<Pixel>::size_type x = 0; x < static_cast<std::vector<Pixel>::size_type>(width); ++x) {
            const Pixel &p1 = image1[y][x];
            const Pixel &p2 = image2[y][x];

            int diffR = std::abs(p1.r - p2.r);
            int diffG = std::abs(p1.g - p2.g);
            int diffB = std::abs(p1.b - p2.b);

            // Comprobar si no se cumple el umbral de 5
            if (diffR >= threshold || diffG >= threshold || diffB >= threshold) {
                imagesAreEqual = false;
                std::cout << "Significant difference at pixel (" << x << ", " << y << "): "
                          << "Image 1 = (" << static_cast<int>(p1.r) << ", " << static_cast<int>(p1.g) << ", " << static_cast<int>(p1.b) << "), "
                          << "Image 2 = (" << static_cast<int>(p2.r) << ", " << static_cast<int>(p2.g) << ", " << static_cast<int>(p2.b) << ")\n";
            }
        }
    }

    if (imagesAreEqual) {
        std::cout << "Las imágenes se consideran iguales para el umbral de " << threshold << ".\n";
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <image1.ppm> <image2.ppm>\n";
        return 1;
    }

    std::vector<std::vector<Pixel>> image1, image2;
    int width1, height1, width2, height2;

    if (!loadPPM(argv[1], image1, width1, height1)) {
        return 1;
    }

    if (!loadPPM(argv[2], image2, width2, height2)) {
        return 1;
    }

    if (width1 != width2 || height1 != height2) {
        std::cerr << "Error: Images are not the same size.\n";
        return 1;
    }

    compareImages(image1, image2, width1, height1);

    return 0;
}
