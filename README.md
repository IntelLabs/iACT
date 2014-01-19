This open source toolkit pertains to a software-hardware co-design framework for approximate computing. 
We have developed a compiler, runtime and a hardware (simulated) test-bed to test out approximations on approximate computing friendly applications.
The goal of approximate computation is to improve computational efficiency. 

This toolkit has been tested on Linux box (Fedora 15).

__(1) Building the llvm tool chain__

LLVM has been modified to include pragma annotations that a programmer supplies into the application binary.

To build the llvm tool chain, do the following steps

Requirements: cmake

From the top level directory `<Top-Level>`

cd compiler

mkdir build

cd build

cmake ../llvm-3.3.src

make -j 8

The last step here is to have the llvm path setup in the linux shell environment.

As an example, if you are working in a bash shell do, 

export PATH=`<Top-Level>`/compiler/build/bin:$PATH

to let the shell know where you have built the llvm compiler.

__(2) Building the Pin tool__

Pin tool simulates a noisy hardware. 

For now, there are several different noise models in the pin tool to simulate noisy (approximate) arithmetic and memory operations.

To build the Pin tool:

Requirements: Download and install Pin from 

http://software.intel.com/en-us/articles/pintool-downloads 

Make sure you download the Rev 58423 (Apr 11, 2013) release for IA32 and intel64.

Make sure to download and install this above Pin distribution into `<Top-Level>`/tool folder.

After that copy over the AxC-PINTOOL folder into `<Top-Level>`/tool/pin-2.12-58423-gcc.4.4.7-linux/source/tools

Note: AxC-PINTOOL is the new Pin tool supplied here and `<Top-Level>`/tool/pin-2.12-58423-gcc.4.4.7-linux is the Pin distro that you download and install

Next, build the Pin tool

cd `<Top-Level>`/tool/pin-2.12-58423-gcc.4.4.7-linux/source/tools/AxC-PINTOOL/

and type,

make
 

__(3) Building and running sample workloads__

To test out our approximation schemes, we are providing two sample workloads - Bodytracking benchmark from PARSEC 2.1 suite and a Sobel Filtering kernel. 
As an example, we show how Bodytracking benchmark is amenable to a noisy hardware and Sobel Filtering workload is amenable to approximate memorization scheme.


(3a) To build and run Bodytrack benchmark (Note: some files in src/TrackingBenchmark/ have been annotated with approximation pragmas and this build process 
compiles and embeds this information into the binary as well as produces two files axcFunctions.txt and pxcFunctions.txt that mentions the names of the functions)

From the `<Top-Level>`/, do

cd workloads/bodytrack-PARSEC/


First, download the native input set for bodytrack application from parsec.cs.princeton.edu/download.htm 

Make sure, you download PARSEC 2.1 Native Inputs and copy over the bodytrack tar file into `<Top-Level>`/workloads/bodytrack-PARSEC/inputs/

Next, untar the inputs


cd inputs


tar -xvf input_native.tar 

 
Go back to the bodytrack-PARSEC folder and build the body track source code

 
cd ../src


./configure --prefix=full/path/to/<folder-name>/axc/workloads/bodytrack-PARSEC/BIN


./build_script.sh


This produces the bodytrack binary in bodytrack-PARSEC/BIN/bin


Run the Bodytrack application with "noisy computation and storage AxC knobs"

 
cd  ~/`<Top-Level>`/workloads/bodytrack-PARSEC/


./../tool/pin-2.12-58423-gcc.4.4.7-linux/pin -t ./../../tool/pin-2.12-58423-gcc.4.4.7-linux/source/tools/AxC-PINTOOL/obj-intel64/axcAnyl.so -d1c 16 -d1b 32 -d1a 16 -axc_c 1 -axc_b 32 -axc_a 16 -u2c 512 -u2b 32 -u2a 16 -axc src/TrackingBenchmark/axcFunctions.txt -- ./BIN/bin/bodytrack inputs/sequenceB_261 4 2 4000 5 4 1

 
The above command executes 2 frames of the native example and runs for about 5-7 mins on a high end machine. 
To run for all the 260 input frames, change the second argument after inputs to 260
 

The parameters to the Pin tool are the cache configurations of the main core’s cache, the approximate core’s cache and the last level cache and a list of functions in the binary on which to do the approximations.


(3b) To build and run Sobel filter


cd  ~/`<Top-Level>`/workloads/Sobel_Filter/

To simply compile and try out the application, do 

gcc -o sobel sobel_filter.c

To run, do

./sobel inputs/baboon.pgm baboon_outputPrecise.pgm

To compile with clang and embed anontations and have memorization flags set, do

clang -c -emit-llvm sobel_filter.c -o test.bc

opt -axc -memo-flag=true test.bc -o test.opt.bc

clang test.opt.bc -o sobelfilter_clang -lm

To run, do

./sobelfilter_clang inputs/baboon.pgm baboon_outputApproxmemo.pgm


Notes on the approximate memoization runtime framework:


Currently, the approximate memorization is supported as a runtime framework (implemented in llvm). 
We support approximate memoization at function argument level. The assumption here being that the function is idempotent, i.e. it has no side-effects and only works on the input variables being passed and changes only the output variables being asked to.
For the current implementation, approximate memoization is supported with a “pragma axc_memoize” to which the programmer specifies the function argument number and the tolerance.
For example, for the Sobel filter code, when calling the kernel_filter, we pass through the pragma tolerance limits of 5% on variables 0 through 8 (the 9 pixels values of the window) and explicitly call out the output variable number (9 in this case).
The runtime framework would build a table of  9 input and 1-output pair, and every time the inputs are within the tolerance limits would read out the value from the table instead of computing the function (hence saving time and computation resources).
In the future, we plan on extending this approximate memoization framework as a Pin tool (to emulate memoization in hardware).

For questions and suggestions, please send email to asit.k.mishra@intel.com
