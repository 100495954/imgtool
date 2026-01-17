# ImgTool - Utilidad de Procesamiento de Imágenes

Una herramienta C++ de alto rendimiento para procesamiento de imágenes desarrollada como proyecto de un curso de arquitectura. Este proyecto implementa dos paradigmas de estructuras de datos diferentes (Array of Structs vs. Struct of Arrays) para comparar características de rendimiento en tareas de manipulación de imágenes.

## Autores

- Mario Cámara - 100495734
- Claudia Alonso - 100495908
- Carmen Serrano . 100495711
- Pablo Lorenzo - 100495954

## Descripción del Proyecto

ImgTool es una utilidad de línea de comandos para procesar imágenes en formato PPM (Portable Pixmap). El proyecto explora cómo diferentes diseños de memoria afectan el rendimiento en operaciones de procesamiento de imágenes proporcionando dos implementaciones paralelas:

- **AOS (Array of Structs)**: Enfoque tradicional que almacena cada píxel como una estructura completa
- **SOA (Struct of Arrays)**: Enfoque moderno que almacena arrays separados para cada canal de color

## Características

La herramienta admite las siguientes operaciones de procesamiento de imágenes:

### 1. **Compress**
Reduce el tamaño de archivo de imágenes PPM preservando la calidad de la imagen mediante optimización de paleta de colores y análisis de frecuencia.

### 2. **Resize**
Escala imágenes a dimensiones especificadas. Admite tanto operaciones de ampliación como reducción.

### 3. **MaxLevel**
Normaliza el rango de color de una imagen según un nivel de color máximo especificado. Útil para extender la profundidad de color y mejorar el contraste de la imagen.

### 4. **CutFreq**
Filtra colores según el umbral de frecuencia. Elimina colores de baja frecuencia (aquellos que aparecen menos frecuentemente) de la imagen, creando un efecto posterizado.

### 5. **Info**
Muestra información detallada sobre una imagen incluyendo:
- Dimensiones (ancho × alto)
- Valor máximo de color
- Número de colores únicos
- Estadísticas de frecuencia de colores

## Estructura del Proyecto

```
imgtool/
├── common/              # Utilidades compartidas
│   ├── binario.cpp/hpp  # Operaciones de E/S de archivos binarios
│   ├── info.cpp/hpp     # Utilidades de información de imágenes
│   └── progargs.cpp/hpp # Análisis de argumentos de línea de comandos
├── imgaos/              # Implementación AOS (Array of Structs)
│   ├── imageaos.cpp/hpp # Procesamiento de imágenes usando estructura de datos AOS
│   └── CMakeLists.txt
├── imgsoa/              # Implementación SOA (Struct of Arrays)
│   ├── imagesoa.cpp/hpp # Procesamiento de imágenes usando estructura de datos SOA
│   └── CMakeLists.txt
├── imtool-aos/          # Ejecutable AOS
│   ├── main.cpp
│   └── CMakeLists.txt
├── imtool-soa/          # Ejecutable SOA
│   ├── main.cpp
│   └── CMakeLists.txt
├── utest-common/        # Pruebas unitarias para utilidades comunes
│   ├── utest-common.cpp
│   └── CMakeLists.txt
├── utest-imgaos/        # Pruebas unitarias para implementación AOS
│   ├── unitest-imgaos.cpp
│   └── CMakeLists.txt
├── utest-img-soa/       # Pruebas unitarias para implementación SOA
│   ├── utest-img-soa.cpp
│   └── CMakeLists.txt
├── ftest-aos/           # Pruebas funcionales para AOS
│   ├── ftest-compress.cpp
│   ├── tester.cpp
│   └── compare_*.sh
├── ftest-soa/           # Pruebas funcionales para SOA
│   ├── ftest-compress.cpp
│   ├── tester.cpp
│   └── compare_*.sh
├── input/               # Imágenes de entrada de muestra
│   └── *.ppm
├── output/              # Imágenes de salida generadas
│   └── (archivos de salida)
├── CMakeLists.txt       # Configuración de compilación principal
├── run_and_build.sh     # Script de compilación y ejecución de pruebas
└── README.md            # Este archivo
```

## Compilación del Proyecto

### Requisitos Previos

- **CMake** 3.29 o superior
- **Compilador compatible con C++20** (GCC, Clang, MSVC)
- **Google Test** (obtenido automáticamente vía FetchContent)
- **Microsoft GSL** (Guidelines Support Library, obtenido automáticamente vía FetchContent)
- **clang-tidy** (opcional, para verificaciones de calidad de código)

### Instrucciones de Compilación

**Usando el script proporcionado:**
```bash
./run_and_build.sh
```

**Compilación manual:**
```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

Los ejecutables compilados se ubicarán en:
- `build-release/imtool-aos/imtool-aos` (versión AOS)
- `build-release/imtool-soa/imtool-soa` (versión SOA)

## Uso

### Sintaxis del Comando

```bash
./imtool-<variante> <archivo_entrada> <archivo_salida> <operación> [parámetros]
```

Donde `<variante>` es `aos` o `soa`.

### Ejemplos

**Comprimir una imagen (AOS):**
```bash
./build-release/imtool-aos/imtool-aos input/lake.ppm output/lake-compressed.cppm compress
```

**Redimensionar una imagen a 800×600 píxeles (SOA):**
```bash
./build-release/imtool-soa/imtool-soa input/lake.ppm output/lake-resized.ppm resize 800 600
```

**Aplicar corte de frecuencia (mantener colores que aparecen más de 50000 veces):**
```bash
./build-release/imtool-aos/imtool-aos input/image.ppm output/filtered.ppm cutfreq 50000
```

**Normalizar niveles de color a profundidad de 16 bits (valor máximo 65535):**
```bash
./build-release/imtool-soa/imtool-soa input/image.ppm output/normalized.ppm maxlevel 65535
```

**Mostrar información de la imagen:**
```bash
./build-release/imtool-aos/imtool-aos input/image.ppm dummy.ppm info
```

## Comparación de Estructuras de Datos

### Array of Structs (AOS)
```cpp
struct Pixel {
    int r, g, b;
};
std::vector<Pixel> pixels;
```
**Características:**
- Canales de color secuenciales por píxel
- Mejor localidad de cache para acceso de píxel único
- Menos eficiente para operaciones por canal
- Diseño de memoria tradicional

### Struct of Arrays (SOA)
```cpp
struct ImageSOA {
    std::vector<int> r, g, b;
};
```
**Características:**
- Canales de color separados
- Mejor eficiencia de cache para operaciones de canal en masa
- Amigable con vectorización
- Más eficiente para algoritmos que procesan canales específicos

## Detalles de Implementación

### Componentes Principales

**Representación de Imágenes:**
- Soporte de formato PPM (P6 binario y P3 ASCII)
- Datos de píxeles almacenados en vectores para gestión de memoria dinámica
- Soporte para profundidades de color variables (8-bit y 16-bit)

**Algoritmos:**
- Análisis de frecuencia de colores usando mapas hash/mapas desordenados
- Redimensionamiento de imágenes usando interpolación
- Normalización del espacio de color
- Optimización de paleta

**Calidad del Código:**
- Conformidad con el estándar C++20
- GSL (Guidelines Support Library) para prácticas seguras de C++
- Manejo exhaustivo de errores
- Integración de clang-tidy para análisis estático
- Cobertura de pruebas unitarias y funcionales

## Pruebas

El proyecto incluye suites de pruebas exhaustivas:

**Pruebas Unitarias:**
```bash
./build-release/utest-common/utest-common
./build-release/utest-imgaos/unitest-imgaos
./build-release/utest-img-soa/utest-img-soa
```

**Pruebas Funcionales:**
```bash
cd ftest-aos && ./tester.cpp
cd ftest-soa && ./tester.cpp
```

Las pruebas validan la corrección del procesamiento de imágenes y comparan resultados entre implementaciones AOS y SOA.

## Perfilado de Rendimiento

El script `run_and_build.sh` incluye ejemplos de perfilado de rendimiento usando `perf` con métricas de energía:

```bash
perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos input.ppm output.ppm compress
```

Esto permite análisis comparativo de:
- Tiempo de ejecución
- Consumo de energía
- Comportamiento de cache
- Utilización del ancho de banda de memoria

## Banderas del Compilador

El proyecto se compila con banderas estrictas del compilador:
- `-Wall -Wextra -Wpedantic` - Habilitar advertencias exhaustivas
- `-Werror` - Tratar advertencias como errores
- `-Wconversion -Wsign-conversion` - Advertir sobre conversiones de tipo
- `-fPIC` - Código independiente de posición

## Formatos Admitidos

**Entrada:**
- Formato PPM (Portable Pixmap), tanto P3 (ASCII) como P6 (binario)

**Salida:**
- Formato PPM (estándar)
- Formato CPPM (formato PPM personalizado comprimido)

## Notas de Desarrollo

- El proyecto es parte de un curso de arquitectura de computadoras enfocado en optimización de estructuras de datos
- Ambas implementaciones AOS y SOA producen resultados idénticos
- Las diferencias de rendimiento destacan la importancia del diseño de memoria en procesadores modernos
- El código sigue prácticas de C++20 con énfasis en seguridad y rendimiento



