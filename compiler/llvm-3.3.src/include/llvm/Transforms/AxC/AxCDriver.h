//==============================================================================
//Copyright (c) 2013 Intel Corporation.
//All rights reserved.
//==============================================================================
//
//Developed by:
//
//    Programming Systems Lab, Intel Labs
//
//    Intel Corporation
//
//    http://www.intel.com
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of
//this software and associated documentation files (the "Software"), to deal with
//the Software without restriction, including without limitation the rights to
//use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimers.
//
//    * Redistributions in binary form must reproduce the above copyright notice,
//      this list of conditions and the following disclaimers in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the names of the LLVM Team, University of Illinois at
//      Urbana-Champaign, nor the names of its contributors may be used to
//      endorse or promote products derived from this Software without specific
//      prior written permission.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
//CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
//SOFTWARE.
//==============================================================================

#ifndef LLVM_TRANSFORMS_AXC_DRIVER_H
#define LLVM_TRANSFORMS_AXC_DRIVER_H
#include "llvm/PassManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Pass.h"
//#include "llvm/Transforms/AxC/AxCTransform.h"

extern ModulePass *createAxCTransfromPass();

static cl::opt<bool>
axc_f("axc", cl::desc("Generate code for Approximate Computing"));

namespace llvm {

class Pass;

static inline void addMyPass(PassManagerBase &PM, Pass *P) {
  // Add the pass to the pass manager...
  PM.add(P);

  // If we are verifying all of the intermediate steps, add the verifier...
  /*if (VerifyEach)*/ PM.add(createVerifierPass());
}

static void axc_driver(PassManager &Passes) {
	if (axc_f) {
		addMyPass(Passes, createAxCTransfromPass());
	}
}
} // End llvm namespace

#endif
