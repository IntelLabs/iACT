#!/bin/sh
#original makefile command
#if g++ -DHAVE_CONFIG_H -I. -I. -I..  -I../FlexImageLib   -g -O2 -Wall -Wno-unknown-pragmas -MT ImageMeasurements.o -MD -MP -MF ".deps/ImageMeasurements.Tpo" -c -o ImageMeasurements.o ImageMeasurements.cpp; \
#    then mv -f ".deps/ImageMeasurements.Tpo" ".deps/ImageMeasurements.Po"; else rm -f ".deps/ImageMeasurements.Tpo"; exit 1; fi

echo "removing old axcFunctions.txt, pxcFunctions.txt and other temporary clang and llvm generated files"
rm -rf axcFunctions.txt
rm -rf pxcFunctions.txt
rm ImageMeasurements.cpp.bc
rm ImageMeasurements.cpp.opt.bc

echo "removing old axcFunctions.txt and pxcFunctions.txt"
clang -DHAVE_CONFIG_H -I. -I. -I..  -I../FlexImageLib   -g -O2 -Wall -Wno-unknown-pragmas -MT -MD -MP -MF ".deps/ImageMeasurements.Tpo"  -c -emit-llvm ImageMeasurements.cpp -o "ImageMeasurements.cpp.bc"
#llvm-dis "ImageMeasurements.cpp.bc"
#opt -axc "ImageMeasurements.cpp.bc" -o "ImageMeasurements.cpp.opt.bc" -debug
opt -axc "ImageMeasurements.cpp.bc" -o "ImageMeasurements.cpp.opt.bc"
#llvm-dis "ImageMeasurements.cpp.opt.bc"
clang  -c "ImageMeasurements.cpp.opt.bc" -o "ImageMeasurements.o"

#clang  -c -DHAVE_CONFIG_H -I. -I. -I..  -I../FlexImageLib   -g -O2 -Wall -Wno-unknown-pragmas -MT -MD -MP -MF ".deps/ImageMeasurements.Tpo" "ImageMeasurements.cpp.opt.bc" -o "ImageMeasurements.o" -lm
