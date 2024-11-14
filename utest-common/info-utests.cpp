//
// Created by claud on 14/11/2024.
//
#include <gtest/gtest.h>
#include "common/info.hpp"
#include <sstream>
#include <fstream>

namespace common::test {

    class InfoTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // Código de configuración si es necesario
        }

        void TearDown() override {
            // Código de limpieza si es necesario
        }
    };

    TEST_F(InfoTest, InfoFunction_ValidInput) {
        // Simular archivo de entrada
        std::string const inputFileContent = "P6 4 4 255\n";

        // Crear un archivo temporal para simular el archivo de entrada
        std::ofstream tempFile("input.ppm");
        tempFile << inputFileContent;
        tempFile.close();

        // Simular parámetros
        progargsCommon::parameters_files params;
        params.input_file = "input.ppm";
        params.output_file = "output.ppm";

        // Redirigir std::cout a un stringstream
        std::ostringstream const outputStream;
        std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
        std::cout.rdbuf(outputStream.rdbuf());

        // Llamar a la función
        info(params);

        // Restaurar std::cout
        std::cout.rdbuf(oldCoutStreamBuf);

        // Verificar la salida
        std::string const expectedOutput = "Input: input.ppm\nOutput: output.ppm\nOperation: info\nImage size: 4x4\nMax level: 255";
        EXPECT_EQ(outputStream.str(), expectedOutput);
    }

    TEST_F(InfoTest, InfoFunction_InvalidInput) {
        // Simular archivo de entrada con contenido inválido
        std::string const inputFileContent = "Invalid content";

        // Crear un archivo temporal para simular el archivo de entrada
        std::ofstream tempFile("input.ppm");
        tempFile << inputFileContent;
        tempFile.close();

        // Simular parámetros
        progargsCommon::parameters_files params;
        params.input_file = "input.ppm";
        params.output_file = "output.ppm";

        // Redirigir std::cerr a un stringstream
        std::ostringstream const errorStream;
        std::streambuf* oldCerrStreamBuf = std::cerr.rdbuf();
        std::cerr.rdbuf(errorStream.rdbuf());

        // Llamar a la función
        info(params);

        // Restaurar std::cerr
        std::cerr.rdbuf(oldCerrStreamBuf);

        // Verificar la salida de error
        std::string const expectedErrorOutput = "Invalid PPM format\n";
        EXPECT_EQ(errorStream.str(), expectedErrorOutput);
    }

}  // namespace common::test

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}