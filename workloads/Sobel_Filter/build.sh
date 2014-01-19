#!/bin/sh
clang -c  -emit-llvm $1 -o "$1.bc"
llvm-dis "$1.bc"
opt -axc -memo-flag=true "$1.bc" -o "$1.opt.bc" -debug
llvm-dis "$1.opt.bc"
clang  "$1.opt.bc" -o "$1.clangout" -lm
llc "$1.opt.bc" -o "$1.s"
g++ "$1.s" -o "$1.gccout" -lm
