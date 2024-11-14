#!/bin/sh

#Para ejecutar quitar el simbolo de comentario del comando a ajecutar

cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Relase
cmake --build build-release

#TESTS COMPRESS AOS
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/compress-lake-small-aos.cppm compress
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-large.ppm ./output/compress-lake-large-aos.cppm compress
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/deer-small.ppm ./output/compress-deer-small-aos.cppm compress
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/deer-large.ppm ./output/compress-deer-large-aos.cppm compress
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/sabatini.ppm ./output/compress-sabatini-aos.cppm compress

#TESTS CUTFREQ AOS
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-large.ppm ./output/cutfreq-lake-large-aos.ppm cutfreq 100000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/cutfreq-lake-small-aos.ppm cutfreq 100000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/deer-large.ppm ./output/cutfreq-deer-large-aos.ppm cutfreq 100000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/cutfreq-lake-small-aos.ppm cutfreq 100000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/sabatini.ppm ./output/cutfreq-sabatini-aos.ppm cutfreq 100000

#TESTS RESIZE AOS
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/resize-lake-small-aos.ppm resize 8000 8000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-large.ppm ./output/resize-lake-large-aos.ppm resize 8000 8000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/deer-small.ppm ./output/resize-deer-small-aos.ppm resize 8000 8000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/deer-large.ppm ./output/resize-deer-large-aos.ppm resize 8000 8000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/sabatini.ppm ./output/resize-sabatini-aos.ppm resize 8000 8000

#TESTS MAXLEVEL AOS
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-large.ppm ./output/maxlevel-lake-large-aos.ppm maxlevel 65535
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/maxlevel-lake-small-aos.ppm maxlevel 65535
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/deer-large.ppm ./output/maxlevel-deer-large-aos.ppm maxlevel 65535
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/maxlevel-lake-small-aos.ppm maxlevel 65535
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/sabatini.ppm ./output/maxlevel-sabatini-aos.ppm maxlevel 65535

#TESTS INFO AOS
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-small.ppm ./output/info-lake-small-aos.ppm info
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/lake-large.ppm ./output/info-lake-large-aos.ppm info
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/deer-small.ppm ./output/info-deer-small-aos.ppm info
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/deer-large.ppm ./output/info-deer-large-aos.ppm info
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos ./input/sabatini.ppm ./output/info-sabatini-aos.ppm info

#############################################################################################################################################################################################

#TESTS COMPRESS SOA
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/compress-lake-small-soa.cppm compress
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-large.ppm ./output/compress-lake-large-soa.cppm compress
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/deer-small.ppm ./output/compress-deer-small-soa.cppm compress
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/deer-large.ppm ./output/compress-deer-large-soa.cppm compress
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/sabatini.ppm ./output/compress-sabatini-soa.cppm compress

#TESTS CUTFREQ SOA
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-large.ppm ./output/cutfreq-lake-large-soa.ppm cutfreq 100000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/cutfreq-lake-small-soa.ppm cutfreq 100000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/deer-large.ppm ./output/cutfreq-deer-large-soa.ppm cutfreq 100000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/cutfreq-lake-small-soa.ppm cutfreq 100000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/sabatini.ppm ./output/cutfreq-sabatini-soa.ppm cutfreq 100000

#TESTS RESIZE SOA
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/resize-lake-small-soa.ppm resize 8000 8000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-large.ppm ./output/resize-lake-large-soa.ppm resize 8000 8000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/deer-small.ppm ./output/resize-deer-small-soa.ppm resize 8000 8000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/deer-large.ppm ./output/resize-deer-large-soa.ppm resize 8000 8000
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/sabatini.ppm ./output/resize-sabatini-soa.ppm resize 8000 8000

#TESTS MAXLEVEL SOA
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-large.ppm ./output/maxlevel-lake-large-soa.ppm maxlevel 65535
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/maxlevel-lake-small-soa.ppm maxlevel 65535
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/deer-large.ppm ./output/maxlevel-deer-large-soa.ppm maxlevel 65535
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/maxlevel-lake-small-soa.ppm maxlevel 65535
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/sabatini.ppm ./output/maxlevel-sabatini-soa.ppm maxlevel 65535

#TESTS INFO SOA
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-small.ppm ./output/info-lake-small-soa.ppm info
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/lake-large.ppm ./output/info-lake-large-soa.ppm info
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/deer-small.ppm ./output/info-deer-small-soa.ppm info
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/deer-large.ppm ./output/info-deer-large-soa.ppm info
#perf stat -a -e power/energy-pkg/ ./build-release/imtool-soa/imtool-soa ./input/sabatini.ppm ./output/info-sabatini-soa.ppm info

##