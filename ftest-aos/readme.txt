Prueba Funcional de Comparación de Imágenes tras resize(ftest)

Esta prueba está diseñada para comparar dos imágenes PPM píxel por píxel, permitiendo un margen de hasta 5 unidades de diferencia de color por canal (R, G, B). Verifica si la imagen generada por un programa coincide con la imagen de referencia esperada.

Archivos

- tester.cpp: Este programa en C++ compara dos imágenes en formato PPM e identifica diferencias significativas entre ellas.
- compare_images.sh`: Este script de shell compila y ejecuta `tester.cpp`, luego compara la imagen generada con la imagen de referencia.
- readme.txt`: Instrucciones sobre cómo configurar y ejecutar la prueba.

Requisitos

- **Compilador C++** (por ejemplo, `g++`) para compilar el programa `tester.cpp`.
- **Shell Bash** para ejecutar el script `compare_images.sh`.
- **Imágenes en formato PPM** como archivos de entrada. Coloca la imagen generada y la imagen de referencia en el mismo directorio que `tester.cpp` y `compare_images.sh`, o proporciona el path correcto.

#### Cómo Usar

1. **Generar la Imagen**: Primero, usa tu programa (por ejemplo, `aos`) para generar una imagen de salida en formato PPM. Guarda esta imagen en el mismo directorio ftest.

2. **Colocar las Imágenes**: Asegúrate de que las imagenes de entrada esten corractamente referenciadas.

3. **Ejecutar la Prueba**:
   - Ejecuta el script `compare_images.sh` con el siguiente comando:
     ```bash
     ./compare_images.sh generated_image.ppm reference_image.ppm
     ```
   - El script:
     - Compilará `tester.cpp` en un ejecutable llamado `compare_images`.
     - Ejecutará `compare_images`, que cargará y comparará `generated_image.ppm` y `reference_image.ppm`.
     - Mostrará cualquier diferencia significativa entre los píxeles, donde los canales de color difieran en más de 5 unidades.

4. **Salida e Interpretación**:
   - Si las imágenes son idénticas dentro del margen especificado, verás:
     ```
     Las imágenes se consideran iguales dentro del umbral de 5.
     ```
   - Si se detectan diferencias significativas, se imprimirá la posición de cada píxel diferente junto con los valores de color para ambas imágenes.
   - Después de ejecutarse, el script eliminará el ejecutable compilado `compare_images`.

#### Notas

- **Umbral**: El programa usa un umbral de 5 para cada canal de color (R, G y B). Esto significa que una diferencia de 5 o más unidades en cualquier canal se informará como significativa.
- **Formato de Imagen**: Las imágenes deben estar en formato PPM binario (`P6`) con un valor máximo de color de 255.
- **Dimensiones Iguales**: Ambas imágenes deben tener el mismo ancho y alto. De lo contrario, se informará un error.

---

Este documento sirve como guía para configurar y ejecutar la prueba funcional que verifica la precisión de las imágenes generadas en comparación con las imágenes de referencia, permitiendo una diferencia de color dentro de un margen aceptable.
