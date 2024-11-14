#!/bin/sh

#Para ejecutar quitar el simbolo de comentario del comando a ajecutar

cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Relase
cmake --build build-release

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/compress-aos.cppm compress

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-large.ppm ./output/cutfreq-aos.ppm cutfreq 100000

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/resize-aos.ppm resize 8000 8000

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-large.ppm ./output/maxlevel-aos.ppm maxlevel 65535

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/sabatini.ppm ./output/info-aos.cppm info

#############################################################################################################################################################################################

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/compress-soa.cppm compress

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-large.ppm ./output/cutfreq-soa.ppm cutfreq 100000

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/resize-soa.ppm resize 8000 8000

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-large.ppm ./output/maxlevel-soa.ppm maxlevel 65535

#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/sabatini.ppm ./output/info-soa.cppm info
