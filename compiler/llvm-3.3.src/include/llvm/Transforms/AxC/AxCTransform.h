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


#ifndef LLVM_TRANSFORMS_AXC_TRANSFORM_H
#define LLVM_TRANSFORMS_AXC_TRANSFORM_H

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CallSite.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/CFG.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/ADT/UniqueVector.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Casting.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/AxC/AxCDriverHeader.h"
using namespace llvm;
using namespace std;

namespace {
  struct AxCTransform : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
	DataLayout * TD;

    AxCTransform() : ModulePass(ID), TD(0) {}
	void getAnalysisUsage(AnalysisUsage &AU) const { }
	bool runOnModule(Module &M);

private:
	struct CompareFirstElement {
		CompareFirstElement(unsigned ArgNo_) : ArgNo(ArgNo_) {}
		bool operator()(const pair<unsigned, float> &arg) const {
			return ArgNo == arg.first;
		}
	private:
		unsigned ArgNo;
	};
	Type *global_struct_type_for_data;
	Type *global_struct_type;
	int number_of_output_arguments;
	int number_of_input_arguments;
	int num_func_generated_so_far;

	void create_global_memoization_table_type_per_axc_pragma(Function *f, 
		SmallVector<pair<unsigned, float>, 4> &InErrorBounds, 
		SmallVector<unsigned, 4> &OutErrorBounds);

	bool handle_axc_pragma (Module &M);
	bool handle_memo_axc_pragma(Module &M);
	Function *create_approximate_function(Function *f, Instruction *OldCallInsn, 
		SmallVector<pair<unsigned, float>, 4> &InErrorBounds, 
		SmallVector<unsigned, 4> &OutErrorBounds, 
		GlobalVariable *global_table_per_axc_pragma,
		Function *initFunc);

	void create_signatures(Module &M);
	void allocate_global_memoization_table_per_axc_pragma(Module &M, GlobalVariable *global_table_per_axc_pragma,
		IRBuilder<> &Builder);	
 };
}

char AxCTransform::ID = 0;
ModulePass  *llvm::createAxCTransfromPass() { return new AxCTransform(); }
//static RegisterPass<AxCTransform> X("axc", "AxC Transformation Pass");

#endif