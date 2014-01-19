
clang.exe -IC:\mingw64\include\c++\4.4.7 -IC:\mingw64\x86_64-w64-mingw32\include -IC:\mingw64\include\c++\4.4.7\x86_64-w64-mingw32 -c -emit-llvm %1 -o %1.bc
llvm-dis %1.bc
opt -axc -memo-flag=true  %1.bc -o %1.opt.bc -debug
llvm-dis %1.opt.bc
llc -mtriple=x86_64-w64-mingw32 %1.opt.bc -o %1.s
g++ -m64 %1.s -o %1.exe
