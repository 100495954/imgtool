//
// Created by claud on 14/11/2024.
//
#include "common/info.hpp"
#include "common/progargs.hpp"

#include <common/binario.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

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

  TEST(ProgArgsCommonTest, CheckArgc) {
    int argc1 = 3;
    int argc2 = 4;
    EXPECT_FALSE(progargsCommon::check_argc(argc1));
    EXPECT_TRUE(progargsCommon::check_argc(argc2));
}

// Test para progargsCommon::pertenencia
TEST(ProgArgsCommonTest, Pertenencia) {
    EXPECT_TRUE(progargsCommon::pertenencia("compress"));
    EXPECT_TRUE(progargsCommon::pertenencia("resize"));
    EXPECT_FALSE(progargsCommon::pertenencia("invalid"));
}

// Test para progargsCommon::args_checker
TEST(ProgArgsCommonTest, ArgsChecker) {
    std::vector<std::string> const args1 = {"program", "file1", "file2", "compress"};
    std::vector<std::string> const args2 = {"program", "file1", "file2", "resize", "width", "height"};
    std::vector<std::string> const args3 = {"program", "file1", "file2", "maxlevel", "level"};
    std::vector<std::string> const args4 = {"program", "file1", "file2", "cutfreq", "frequency"};
    std::vector<std::string> const invalidArgs = {"program", "file1", "file2", "invalid"};

    EXPECT_TRUE(progargsCommon::args_checker(args1));
    EXPECT_TRUE(progargsCommon::args_checker(args2));
    EXPECT_TRUE(progargsCommon::args_checker(args3));
    EXPECT_TRUE(progargsCommon::args_checker(args4));
    EXPECT_FALSE(progargsCommon::args_checker(invalidArgs));
}

// Test para binario::read_binary y binario::write_binary
TEST(BinaryTest, ReadWriteBinary) {
    std::ostringstream output_stream;
    int const input_value = 42;

    // Escribe en el flujo binario
    binario::write_birary(output_stream, input_value);

    // Lee del flujo binario
    std::istringstream input_stream(output_stream.str());
    int const output_value = binario::read_binary<int>(input_stream);

    // Verifica que el valor leído sea igual al escrito
    EXPECT_EQ(input_value, output_value);
}

// Test para binario::read_binary con error
TEST(BinaryTest, ReadBinaryError) {
    std::istringstream empty_stream; // Stream vacío
    EXPECT_THROW(binario::read_binary<int>(empty_stream), std::runtime_error);
}

// Test para binario::write_birary
TEST(BinaryTest, WriteBinary) {
    std::ostringstream output_stream;
    double const input_value = std::numbres::pi;

    binario::write_birary(output_stream, input_value);

    // Verifica que el tamaño del flujo sea igual al tamaño de un double
    EXPECT_EQ(output_stream.str().size(), sizeof(double));
}


}  // namespace common::test

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
