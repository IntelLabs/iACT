echo "make clean and make"
make clean; make
echo "moving build.sh into TrackingBenchmark folder"
cp build.sh TrackingBenchmark/
echo "cd into TrackingBenchmark folder"
cd TrackingBenchmark
echo "generating axcFunctions.txt and pxcFunctions.txt"
./build.sh
cd ../
echo "At top level directory"
make
make install
