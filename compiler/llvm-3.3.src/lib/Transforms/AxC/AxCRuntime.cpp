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

//#define DEBUG_TYPE "axc"
#include "llvm/Transforms/AxC/AxCTransform.h"
#include <fstream>
#include <iostream>

//#define DEBUG_MEMOIZE

bool MEMO_FLAG;
static cl::opt<bool>
	memo_f("memo-flag", cl::init(false), cl::Hidden,
	cl::desc("Generate code for Memoization in Approximate Computing"));

int GLOBAL_TABLE_SIZE;
static cl::opt<int>
	global_table_size("memo-table-size", cl::init(5), cl::Hidden,
	cl::desc("Size of the memoization global table"));

//STATISTIC(AxCCounter, "Counts number of functions greeted");

/* Step 2: using the IN arguments and OUT arguments, find their types and build a global struct */
/* Step 2a: create two counters -- head and tail  for the same table before */
/* Example: struct { int head; int tail; int IN_1, int IN_2, int OUT_1} axc_struct_line_1; */
void AxCTransform::create_global_memoization_table_type_per_axc_pragma(Function *f, 
	SmallVector<pair<unsigned, float>, 4> &InErrorBounds, 
	SmallVector<unsigned, 4> &OutErrorBounds 
	) {
		LLVMContext &ctx = f->getContext();

		vector<Type *> params;
		params.clear();
		vector<Type *> inner_params;
		inner_params.clear();

		/* Check output first */
		unsigned ArgNo = 0, index = 0;
		for (Function::arg_iterator IT = f->arg_begin(), ITE = f->arg_end(); IT != ITE; ++IT, ++ArgNo){
			//DEBUG(dbgs() << "ArgNum=" << ArgNo << "\n");
			SmallVector<unsigned, 4>::iterator it = std::find(OutErrorBounds.begin(), OutErrorBounds.end(), ArgNo);
			if (it != OutErrorBounds.end()) { // found element
				Type *ArgType = IT->getType();
				PointerType *PtrArg;
				if (ArgType->isPointerTy() && (PtrArg = dyn_cast<PointerType>(ArgType)) && 
					(PtrArg->getElementType()->isIntegerTy() || PtrArg->getElementType()->isFloatingPointTy())) {
						//DEBUG(dbgs() << "Next Output type::" << PtrArg->getElementType() << "\n");
						inner_params.push_back(PtrArg->getElementType());
						index++;
				}
			}
		}
		number_of_output_arguments = index;
		if (inner_params.empty()) return;

		/* Now the input bounds */
		ArgNo = 0; index=0;
		for (Function::arg_iterator IT = f->arg_begin(), ITE = f->arg_end(); IT != ITE; ++IT, ++ArgNo){
			SmallVector<pair<unsigned, float>, 4>::iterator it = std::find_if(InErrorBounds.begin(), InErrorBounds.end(), CompareFirstElement(ArgNo));
			
			if (it != InErrorBounds.end()) { // found element
				Type *ArgType = IT->getType();
				if ((!ArgType->isPointerTy()) && (ArgType->isIntegerTy() || ArgType->isFloatingPointTy())) {
					//DEBUG(dbgs() << "Next Input type::" << ArgType << "\n");
					inner_params.push_back(ArgType);
					index++;
				}
			}
		}
		number_of_input_arguments = index;
		if (inner_params.empty()) return;

		/* create the inner struct -- the actual table*/
		global_struct_type_for_data = StructType::get(ctx, inner_params, false); 
		//((StructType *)global_struct_type_for_data)->setName("global_struct_data_s");

		/* push integer for head and tail */
		params.push_back(Type::getInt32Ty(ctx)); // head
		params.push_back(Type::getInt32Ty(ctx)); // tail
		params.push_back(PointerType::get(global_struct_type_for_data, 0)); // pointer to data

		/* create the struct */
		global_struct_type = StructType::get(ctx, params, false); 
		//((StructType *)global_struct_type)->setName("global_struct_s");
}

/* Step 3: in the main method, at entry malloc a fixed size table for each type -- associate that to the call site */
void AxCTransform::allocate_global_memoization_table_per_axc_pragma(Module &M, 
	 GlobalVariable *global_table_per_axc_pragma,
	 IRBuilder<> &Builder){

		 LLVMContext &ctx = M.getContext();
		 int global_struct_size=0;
		 for(unsigned int j = 0;j < global_struct_type->getNumContainedTypes();j++){
			 Type *ty = global_struct_type->getContainedType(j);
			 int t_size = TD->getTypeStoreSizeInBits(ty); 
			 assert(t_size != 0);
			 if(t_size==0){
				 DEBUG(dbgs()<<"Error:: Size Can't be 0\n");
				 ty->dump();
			 }
			 global_struct_size += t_size;
		 }
		 global_struct_size /= 8; // in bytes

		 DEBUG(dbgs() << "size of global_struct=" << global_struct_size << " bytes\n");
		 Function *malloc_f = M.getFunction("malloc");
		 assert(malloc_f != NULL);

		 Function::arg_iterator malloc_f_iterator = malloc_f->arg_begin();
		 Argument *malloc_size_arg = &*malloc_f_iterator;
		 Constant *size = ConstantInt::get(malloc_size_arg->getType(), global_struct_size);
		 SmallVector<Value*, 1> Args;
		 Args.clear();	
		 Args.push_back(size);

		 /* malloc the memory region for the global table */
		 /*  %call = tail call i8* @malloc(i64 16) #7 */
		 CallInst *malloc_call = Builder.CreateCall(malloc_f, Args, "");
		 malloc_call->setTailCall();
		 
		 /* Cast it to the new struct type */
		 /* %0 = bitcast i8* %call to %struct.global_struct_s* */
		 Value *global_table_malloced = Builder.CreateBitCast(malloc_call, PointerType::get(global_struct_type, 0), "");

		 /* store it to global variable */
		 /*store %struct.global_struct_s* %0, %struct.global_struct_s** @global_table_per_axc, align 8, !tbaa !3*/
		 /*StoreInst *s =*/ Builder.CreateAlignedStore(global_table_malloced, global_table_per_axc_pragma,  TD->getPointerPrefAlignment(), false);	

		 /* store 0 to head and tail */
		 /* initialize tail and head to zeros */
		 /*  %head = getelementptr inbounds i8* %call, i64 8
		 %1 = bitcast i8* %head to i32*
		 store i32 0, i32* %1, align 4, !tbaa !4
		 %tail = getelementptr inbounds i8* %call, i64 12
		 %2 = bitcast i8* %tail to i32*
		 store i32 0, i32* %2, align 4, !tbaa !4
		 */
		 for (int i = 0; i < 2; i++) {
			 int offset = i; //i*4; // TODO hand coded for 32-bit integer
			 Value *Idxs[2] = { ConstantInt::get(Type::getInt64Ty(ctx), 0), ConstantInt::get(Type::getInt32Ty(M.getContext()), offset)};
			 Value *gep = Builder.CreateGEP(global_table_malloced, Idxs, "");
			 //DEBUG(dbgs() << "GEP=");
			 //gep->dump();
			 //Value *BCI = Builder.CreateBitCast(gep, PointerType::get(Type::getInt32Ty(ctx),0), "");
			 Constant *Zero = ConstantInt::getNullValue(Type::getInt32Ty(ctx));
			 //StoreInst *ss = Builder.CreateAlignedStore(Zero, BCI, 4, false);			 // pointer alignment is 4
			 /*StoreInst *ss =*/ Builder.CreateStore(Zero, /*BCI*/gep, false);			 // pointer alignment is 4
		 }

		 /* now malloc the actual data for the table */
		 /*%call1 = tail call i8* @malloc(i64 24) #7 */
		 int global_struct_size_for_data=0;
		 for(unsigned int j = 0;j < global_struct_type_for_data->getNumContainedTypes();j++){
			 Type *ty = global_struct_type_for_data->getContainedType(j);
			 int t_size = TD->getTypeStoreSizeInBits(ty); 
			 assert(t_size != 0);
			 if(t_size==0){
				 DEBUG(dbgs()<<"Error:: Size Can't be 0\n");
				 ty->dump();
			 }
			 global_struct_size_for_data += t_size;
		 }
		 global_struct_size_for_data /= 8; // in bytes

		 global_struct_size_for_data *= GLOBAL_TABLE_SIZE; // allocate all the entry right now
		 DEBUG(dbgs() << "size of global_struct_data=" << global_struct_size_for_data << " bytes\n");

		 Constant *size_for_data = ConstantInt::get(malloc_size_arg->getType(), global_struct_size_for_data);
		 SmallVector<Value*, 1> Args_for_data;
		 Args_for_data.clear();	
		 Args_for_data.push_back(size_for_data);

		 /* malloc the memory region for the global table */
		 /*%call1 = tail call i8* @malloc(i64 24) #7 */
		 CallInst *malloc_call_for_data = Builder.CreateCall(malloc_f, Args_for_data, "");
		 malloc_call_for_data->setTailCall();

		 /*%3 = bitcast i8* %call1 to %struct.global_struct_s_*
		 %data = bitcast i8* %call to %struct.global_struct_s_** */
		 /* Cast it to the new struct type */
		 /* %0 = bitcast i8* %call to %struct.global_struct_s* */
		 Value *global_table_data_malloced = Builder.CreateBitCast(malloc_call_for_data, PointerType::get(global_struct_type_for_data, 0), "");

		 /*
		 %data = getelementptr inbounds i8* %call, i64 8
		 %3 = bitcast i8* %data to %struct.global_struct_s_**
		 */
		 Value *Idxs[2] = { ConstantInt::get(Type::getInt64Ty(ctx), 0), ConstantInt::get(Type::getInt32Ty(M.getContext()), /*8*/2)}; // VERIFY TODO OFFSET
		 Value *gep = Builder.CreateGEP(/*malloc_call*/global_table_malloced, Idxs, "");
		 //Value *BCI = Builder.CreateBitCast(gep, PointerType::get(PointerType::get(global_struct_type_for_data,0),0), "");


		 /* store it to global variable */
		 /*store %struct.global_struct_s_* %3, %struct.global_struct_s_** %data, align 8, !tbaa !3 */
		 /*StoreInst *store =*/ Builder.CreateAlignedStore(global_table_data_malloced, /*BCI*/gep, TD->getPointerPrefAlignment(), false);	//TD->getPointerPrefAlignment();
}

/**
* create an internal signature for fabs and malloc if it is not already there
*/
void AxCTransform::create_signatures(Module &M) {
	LLVMContext &ctx = M.getContext();
	Function *main = M.getFunction("main");
	assert(main != NULL);

	Function *fabs_f = M.getFunction("fabs");
	if (fabs_f == NULL) {
		vector<Type*> Params;
		string name = "fabs";
		Params.clear();
		Params.push_back(Type::getDoubleTy(ctx));
		FunctionType *fabs_Ty = FunctionType::get(Type::getDoubleTy(ctx), Params, false);
		Function *fabs = Function::Create(fabs_Ty, main->getLinkage(), name);
		M.getFunctionList().insert(main, fabs);
	}
	Function *malloc_f = M.getFunction("malloc");
	if (malloc_f == NULL) {
		vector<Type*> Params;
		Params.clear();
		string name = "malloc";
		Params.push_back(Type::getInt64Ty(ctx));
		FunctionType *malloc_Ty = FunctionType::get(PointerType ::get(Type::getInt8Ty(ctx), 0), Params, false);
		Function *malloc = Function::Create(malloc_Ty, main->getLinkage(), name);
		M.getFunctionList().insert(main, malloc);
	}
}

/* Step 4: Generate approximate code:
a) if IN and OUT arguments are not empty, and their error bounds are known
then get the table corresponding to the current call site/LoC
b) Generate code like this:
if (IN_1 is within  error bounds, IN_2 is wihin error bounds, and so on) {
extract output value and store onto the OUT argument;
// ignore the call instruction
} else {
call the call site...
if the total number of items in the table is the max entry size, replace the head entry and update head 
}
//add a phi node for the call return values!
*/
Function *AxCTransform::create_approximate_function(Function *f, 
		Instruction *OldCallInsn, SmallVector<pair<unsigned, float>, 4> &InErrorBounds, 
		SmallVector<unsigned, 4> &OutErrorBounds, 
		GlobalVariable *global_table_per_axc_pragma,
		Function *initializationFunc) {

			Module *M = f->getParent();
			LLVMContext &ctx = f->getContext();

			/* Step 4a: Create a new function from the old one with the same signature*/
			vector</*const*/ Type *> params;
			for (Function::arg_iterator IT = f->arg_begin(), ITE = f->arg_end(); IT != ITE; ++IT)
				params.push_back(IT->getType());

			const FunctionType *FTy = f->getFunctionType();
			Type *RetTy = FTy->getReturnType();
			FunctionType *NFty = FunctionType::get(RetTy,params, false);
			string new_f_name(f->getName());
			new_f_name += "_approx_";
#if 0
			new_f_name += std::to_string(num_func_generated_so_far++);
#endif
			Function *new_f = Function::Create(NFty, f->getLinkage(), new_f_name.c_str());
			new_f->copyAttributesFrom(f);
			f->getParent()->getFunctionList().insert(f, new_f);

			int counter = 0;
			for (Function::arg_iterator IT = new_f->arg_begin(), ITE = new_f->arg_end(); IT != ITE; ++IT, counter++) {
				string name("arg_");
#if 0
				name += std::to_string(counter);
#endif
				IT->setName(name.c_str());
			}

			/* generate the code sequence that does the following:

			void func_approx_1(int in1, int in2, double *out1, double *out2) {
			int head = global_table_per_axc->head;
			int tail = global_table_per_axc->tail;
			for(int idx = head; idx != tail; idx = (idx+1) %  MAX_TABLE_ENTRY ) {

			//obtain from global table 
			int table_in1 = global_table_per_axc->data[idx].in1;
			//ensure that table_in1 is not zero 
			float current_err1 = ((abs(table_in1 - in1))/table_in1)*100;
			float required_err1 = error_percentage;

			// obtain from global table 
			int table_in2 = global_table_per_axc->data[idx].in2;
			// ensure that table_in1 is not zero 
			float current_err2 = ((abs(table_in2 - in2))/table_in2)*100;
			float required_err2 = error_percentage;

			// provably good approximation 
			if(current_err1 <= required_err1 && current_err2 <= required_err2) {
			*out1 = global_table_per_axc->data[idx].out1;
			*out2 = global_table_per_axc->data[idx].out2;
			return;
			} 
			}

			// precise executon and saving results 
			func(in1, in2, out1, out2);

			tail = (tail + 1) % MAX_TABLE_ENTRY;
			head = (tail == head) ? (tail + 1) % MAX_TABLE_ENTRY : head;

			global_table_per_axc->data[tail].in1 = in1;
			global_table_per_axc->data[tail].in2 = in2;
			global_table_per_axc->data[tail].out1 = *out1;
			global_table_per_axc->data[tail].out2 = *out2;

			global_table_per_axc->head = head;
			global_table_per_axc->tail = tail;

			}
			*/

			BasicBlock *PreEntryBB = BasicBlock::Create(ctx, "", new_f);
			PreEntryBB->setName("entry");

			BasicBlock *PreEntryIfBB = BasicBlock::Create(ctx, "", new_f);
			PreEntryIfBB->setName("entry.if");

			BasicBlock *EntryBB = BasicBlock::Create(ctx, "", new_f);
			EntryBB->setName("real.entry");

			BasicBlock *LoopPreHeaderBB = BasicBlock::Create(ctx, "", new_f);
			LoopPreHeaderBB->setName("for.body.lr.ph");

			BasicBlock *LoopBodyBB = BasicBlock::Create(ctx, "", new_f);
			LoopBodyBB->setName("for.body");

			BasicBlock *LoopExitBB = BasicBlock::Create(ctx, "", new_f);
			LoopExitBB->setName("for.end");

			BasicBlock *LoopIncBB = BasicBlock::Create(ctx, "", new_f);
			LoopIncBB->setName("for.inc");

			BasicBlock *FirstIfBB = BasicBlock::Create(ctx, "", new_f);
			FirstIfBB->setName("if.end");

			BasicBlock *SecondIfBB = BasicBlock::Create(ctx, "", new_f);
			SecondIfBB->setName("if.then");


			BasicBlock *ReturnBB = BasicBlock::Create(ctx, "", new_f);
			ReturnBB->setName("return");

			BasicBlock *LoopExitCondTrueBB = BasicBlock::Create(ctx, "", new_f);
			LoopExitCondTrueBB->setName("cond.true");

			BasicBlock *LoopExitCondFalseBB = BasicBlock::Create(ctx, "", new_f);
			LoopExitCondFalseBB->setName("cond.false");

#ifdef DEBUG_MEMOIZE
			Function *printf_f = M->getFunction("printf");
#endif

			/* Block that checks if axc is null or not */
			IRBuilder<> PreEntryBBBuilder(PreEntryBB);
			LoadInst *pre_global_table_ptr = PreEntryBBBuilder.CreateAlignedLoad(global_table_per_axc_pragma, TD->getPointerPrefAlignment(), false, "pre_global_table");
			Value *precomp = PreEntryBBBuilder.CreateICmpEQ(pre_global_table_ptr, 
				ConstantPointerNull::get(PointerType::get(global_struct_type, 0)));
			BranchInst *prebr = PreEntryBBBuilder.CreateCondBr(precomp, PreEntryIfBB, EntryBB);
			

			/* Block that calls the initialization routine */
			IRBuilder<> PreEntryIfBBBuilder(PreEntryIfBB);
			SmallVector<Value *, 1> arg_init_params; arg_init_params.clear();
			Value *init_func_call = PreEntryIfBBBuilder.CreateCall(initializationFunc, arg_init_params, "");
			cast<CallInst>(init_func_call)->setTailCall();
			//init_func_call->dump();
			PreEntryIfBBBuilder.CreateBr(EntryBB);



			/* real entry block: load head and tail

			entry:
			%0 = load %struct.global_struct_s** @global_table_per_axc, align 8, !tbaa !3
			%head1 = getelementptr inbounds %struct.global_struct_s* %0, i64 0, i32 0
			%1 = load i32* %head1, align 4, !tbaa !4
			%tail2 = getelementptr inbounds %struct.global_struct_s* %0, i64 0, i32 1
			%2 = load i32* %tail2, align 4, !tbaa !4

			%cmp79 = icmp eq i32 %1, %2
			br i1 %cmp79, label %for.end, label %for.body.lr.ph
			*/

			/*
			%0 = load %struct.global_struct_s** @global_table_per_axc, align 8, !tbaa !3
			*/
			IRBuilder<> EntryBBBuilder(EntryBB);
			LoadInst *global_table_ptr = EntryBBBuilder.CreateAlignedLoad(global_table_per_axc_pragma, TD->getPointerPrefAlignment(), false, "global_table");
			/*
			%data = getelementptr inbounds %struct.global_struct_s* %0, i64 0, i32 2
			%3 = load %struct.global_struct_s_** %data, align 8, !tbaa !3
			*/
			Value *Idxsss[2] = {ConstantInt::get(Type::getInt64Ty(ctx), 0),  ConstantInt::get(Type::getInt32Ty(ctx), 2)};
			Value *data_ptr = EntryBBBuilder.CreateGEP(global_table_ptr, Idxsss, "data_ptr");
			LoadInst *data = EntryBBBuilder.CreateAlignedLoad(data_ptr, TD->getPointerPrefAlignment(), false, "data");
				
			/*
			%head1 = getelementptr inbounds %struct.global_struct_s* %0, i64 0, i32 0
			%1 = load i32* %head1, align 4, !tbaa !4
			*/
			Value *Idxs[2] = {ConstantInt::get(Type::getInt64Ty(ctx), 0),  ConstantInt::get(Type::getInt32Ty(ctx), 0)};
			Value *head_ptr = EntryBBBuilder.CreateGEP(global_table_ptr, Idxs, "head_ptr");
			//LoadInst *head = EntryBBBuilder.CreateAlignedLoad(head_ptr, 4, false, ""); //4 bytes alignment 
			LoadInst *head = EntryBBBuilder.CreateLoad(head_ptr, false, "head"); //4 bytes alignment 
			
			/*
			%tail2 = getelementptr inbounds %struct.global_struct_s* %0, i64 0, i32 1
			%2 = load i32* %tail2, align 4, !tbaa !4
			*/
			Value *Idxss[2] = {ConstantInt::get(Type::getInt64Ty(ctx), 0),  ConstantInt::get(Type::getInt32Ty(ctx), 1)};
			Value *tail_ptr = EntryBBBuilder.CreateGEP(global_table_ptr, Idxss, "tail_ptr");
			//LoadInst *tail = EntryBBBuilder.CreateAlignedLoad(tail_ptr, 4, false, ""); //4 bytes alignment 
			LoadInst *tail = EntryBBBuilder.CreateLoad(tail_ptr,false, "tail"); //4 bytes alignment 
			
			/*
				%cmp = icmp slt i32 %2, %1
				%sub = sub nsw i32 %2, %1
				%.sub = select i1 %cmp, i32 5, i32 %sub
				%cmp4117 = icmp sgt i32 %.sub, 0
			*/
			Value *compare_head_tail = EntryBBBuilder.CreateICmpSLT( tail, head, ""); 
			Value *sub = EntryBBBuilder.CreateSub(tail, head, "", false, true);
			Value *select = EntryBBBuilder.CreateSelect(compare_head_tail,
				ConstantInt::get(Type::getInt32Ty(ctx), GLOBAL_TABLE_SIZE),
				sub);
			Value *compr = EntryBBBuilder.CreateICmpSGT( select, ConstantInt::get(Type::getInt32Ty(ctx), 0), ""); 
			BranchInst *br = EntryBBBuilder.CreateCondBr(compr, LoopPreHeaderBB, LoopExitBB);
			

			/*
			%cmp79 = icmp sgt i32 %1, %2
			br i1 %cmp79, label %for.end, label %for.body.lr.ph
			*/
			//Value *compare_head_tail = EntryBBBuilder.CreateICmpSLT( head, tail, ""); 
			///*BranchInst *br = */EntryBBBuilder.CreateCondBr(compare_head_tail, LoopPreHeaderBB, LoopExitBB);

			/*
			for.body.lr.ph:                                   ; preds = %entry

			br label %for.body
			*/
			IRBuilder<> LoopPreHeaderBBBuilder(LoopPreHeaderBB);
			/*BranchInst *br2 = */LoopPreHeaderBBBuilder.CreateBr(LoopBodyBB);


			/* Body of the for loop that loads all input values and checks if they are not zero -- if any is zero, continue the loop
			for.body:                                         ; preds = %for.inc, %for.body.lr.ph
			%idx.086 = phi i32 [ %1, %for.body.lr.ph ], [ %rem, %for.inc ]
			%idxprom = sext i32 %idx.086 to i64
			%in13 = getelementptr inbounds %struct.global_struct_s_* %3, i64 %idxprom, i32 2
			%5 = load i32* %in13, align 4, !tbaa !4
			%in27 = getelementptr inbounds %struct.global_struct_s_* %3, i64 %idxprom, i32 3
			%6 = load i32* %in27, align 4, !tbaa !4
			%cmp8 = icmp eq i32 %5, 0
			%cmp9 = icmp eq i32 %6, 0
			%or.cond = or i1 %cmp8, %cmp9
			br i1 %or.cond, label %for.inc, label %if.end */
			IRBuilder<> LoopBodyBBBuilder(LoopBodyBB);
			PHINode *idx_phi = LoopBodyBBBuilder.CreatePHI(Type::getInt32Ty(ctx), 2, "idx.088");
			idx_phi->addIncoming(head,LoopPreHeaderBB);

			PHINode *num_elements_phi = LoopBodyBBBuilder.CreatePHI(Type::getInt32Ty(ctx), 2, "num_elements.0119");
			num_elements_phi->addIncoming(select,LoopPreHeaderBB);
			
			Value *idx = LoopBodyBBBuilder.CreateSExt(idx_phi, Type::getInt64Ty(ctx), "idxprom");
			
			/*load all in values and check if they are zeroes */
			Value **ins = (Value **)malloc(number_of_input_arguments*sizeof(Value *));
			Value **cmps = (Value **)malloc(number_of_input_arguments*sizeof(Value *));
			//fprintf(stderr, "output=%d input=%d\n", number_of_output_arguments, number_of_input_arguments);

#ifdef DEBUG_MEMOIZE
			//@.str = private unnamed_addr constant [27 x i8] c"Returned from table at=%d\0A\00", align 1
			//%call31 = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([27 x i8]* @.str, i64 0, i64 0), i32 %idx.0100) #8
			if (printf_f != NULL) {
				Constant *stringConstant = ConstantDataArray::getString(ctx, StringRef("Inside for-loop-body\n"), true);
				GlobalValue *stringGlobal =new GlobalVariable(*M, stringConstant->getType(), false, GlobalValue::PrivateLinkage, stringConstant,"");
				SmallVector<Value*, 4> arg_printf_params;
				arg_printf_params.clear();
				Value *Idxz[2] = {ConstantInt::get(Type::getInt64Ty(ctx), 0),  ConstantInt::get(Type::getInt64Ty(ctx), 0)};
				Value *stringGlobal_ptr = LoopBodyBBBuilder.CreateGEP(stringGlobal, Idxz, "");
				Value *arg_1=LoopBodyBBBuilder.CreateCast(Instruction::BitCast, stringGlobal_ptr, PointerType::get(Type::getInt8Ty(ctx), 0));
				arg_printf_params.push_back(arg_1);
				Value *printf_func_call = LoopBodyBBBuilder.CreateCall(printf_f, arg_printf_params, "");
				cast<CallInst>(printf_func_call)->setTailCall();
			}
#endif
			for(int i=0; i<number_of_input_arguments; i++) {
				Value *Idxzz[2] = {idx,
					ConstantInt::get(Type::getInt32Ty(ctx), number_of_output_arguments+i)};
				Value *in_ptr = LoopBodyBBBuilder.CreateGEP(data, Idxzz, "");
				//ins[i] = LoopBodyBBBuilder.CreateAlignedLoad(in_ptr, 4, false, "");
				ins[i] = LoopBodyBBBuilder.CreateLoad(in_ptr, false, "");
				if(ins[i]->getType()->isIntegerTy()) {
					cmps[i] = LoopBodyBBBuilder.CreateICmpEQ(ins[i], ConstantInt::get(ins[i]->getType(), 0), "");
				} else if (ins[i]->getType()->isFloatingPointTy()) {
					cmps[i] = LoopBodyBBBuilder.CreateFCmpOEQ(ins[i], ConstantFP::get(ins[i]->getType(), 0), "");  // FIXME:: Should this really be float or double type?
				} else {
					llvm_unreachable(" Currently not handled\n");
				}
			}
			Value *final_or_cond = cmps[0];
			for(int i=1; i<number_of_input_arguments; i++) {
				Value *orval = LoopBodyBBBuilder.CreateOr(cmps[i], final_or_cond,"");
				final_or_cond = orval;
			}
			LoopBodyBBBuilder.CreateCondBr(final_or_cond, LoopIncBB, FirstIfBB);

			/* Compute the actual error vs. the error threshold and see if it is acceptable, if so, return the output from previous computations
			%sub = sub nsw i32 %17, %18
			%call = call i32 @abs(i32 %sub)
			%19 = load i32* %table_in1, align 4
			%div = sdiv i32 %call, %19
			%mul = mul nsw i32 %div, 100
			%conv = sitofp i32 %mul to float
			*/
			IRBuilder<> FirstIfBBBuilder(FirstIfBB);

			/* Lets always use fabs call, if it is integer, we will cast it to float first */
			Function *fabs_f = f->getParent()->getFunction("fabs");
			assert(fabs_f != NULL);


			/* OPTIMIZE THE CODE GENERATION LATER: FIXME TODO

			TODO; right now, all the conditions for error checking are generated and then anded!
			Ideally you want to evaluate each at a time and check for condition and then branch
			In future, I want to do this 

			*/

			/* this is the list of comparison for the value comparison for actual error vs. error threshold */
			Value **second_cmps = (Value **)malloc(number_of_input_arguments*sizeof(Value *));
			unsigned ArgNo = 0, next_idx=0;
			for (Function::arg_iterator IT = new_f->arg_begin(), ITE = new_f->arg_end(); IT != ITE; ++IT, ++ArgNo){
				SmallVector<pair<unsigned, float>, 4>::iterator it = std::find_if(InErrorBounds.begin(), InErrorBounds.end(), CompareFirstElement(ArgNo));
				if (it != InErrorBounds.end()) { // found element
					Argument *arg = &*IT;
					Value *sub_in_double, *in_in_double;
					if(arg->getType()->isIntegerTy()) {
						Value *sub = FirstIfBBBuilder.CreateSub(ins[next_idx], arg, "");
						sub_in_double = FirstIfBBBuilder.CreateCast(Instruction::SIToFP, sub, Type::getDoubleTy(ctx), "");
						in_in_double = FirstIfBBBuilder.CreateCast(Instruction::SIToFP, ins[next_idx],   Type::getDoubleTy(ctx), "");	
					} else if (arg->getType()->isFloatingPointTy()) {
						Value *sub = FirstIfBBBuilder.CreateFSub(ins[next_idx], arg, "");
						sub_in_double = FirstIfBBBuilder.CreateCast(Instruction::SIToFP, sub, Type::getDoubleTy(ctx), "");
						in_in_double = FirstIfBBBuilder.CreateCast(Instruction::SIToFP, ins[next_idx],   Type::getDoubleTy(ctx), "");;
					} else {
						llvm_unreachable(" Currently not handled\n");
					}

					SmallVector<Value*, 1> Args;
					Args.clear();	
					Args.push_back(sub_in_double);
					CallInst *fabs_call = FirstIfBBBuilder.CreateCall(fabs_f, Args, "");
					fabs_call->setTailCall();
					Value *fdiv = FirstIfBBBuilder.CreateFDiv(fabs_call, in_in_double,"");
					Value *fdiv_trunc = FirstIfBBBuilder.CreateFPTrunc(fdiv, Type::getFloatTy(ctx), "");
					Value *current_error = FirstIfBBBuilder.CreateFMul(fdiv_trunc, ConstantFP::get(Type::getFloatTy(ctx), 100), "");
					float error_threshold = it->second;

					/* %cmp19 = fcmp ole float %25, %26 */
					second_cmps[next_idx]  = FirstIfBBBuilder.CreateFCmpOLE(current_error, ConstantFP::get(Type::getFloatTy(ctx), error_threshold), "");
					next_idx++;
				}
			}

			Value *final_and_cond = second_cmps[0];
			for(int i=1; i<number_of_input_arguments; i++) {
				Value *andval = FirstIfBBBuilder.CreateAnd(cmps[i], final_and_cond,"");
				final_and_cond = andval;
			}
			FirstIfBBBuilder.CreateCondBr(final_and_cond, SecondIfBB, LoopIncBB);

			/* We have found the appropriate entry in the table, we do not execute the precise function now, but we 
			extract the outputs from the table and reurn:::
			if.then23:                                        ; preds = %land.lhs.true
			%out127 = getelementptr inbounds %struct.global_struct_s_* %3, i64 %idxprom, i32 0
			%9 = load double* %out127, align 8, !tbaa !0
			store double %9, double* %out1, align 8, !tbaa !0

			%out231 = getelementptr inbounds %struct.global_struct_s_* %3, i64 %idxprom, i32 1
			%10 = load double* %out231, align 8, !tbaa !0
			store double %10, double* %out2, align 8, !tbaa !0
			br label %return
			*/
			IRBuilder<> SecondIfBBBuilder(SecondIfBB);
			next_idx = 0; ArgNo = 0;
#ifdef DEBUG_MEMOIZE
			//@.str = private unnamed_addr constant [27 x i8] c"Returned from table at=%d\0A\00", align 1
			//%call31 = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([27 x i8]* @.str, i64 0, i64 0), i32 %idx.0100) #8
			if (printf_f != NULL) {
				Constant *stringConstant = ConstantDataArray::getString(ctx, StringRef("Returned from table at=%d\n"), true);
				GlobalValue *stringGlobal =new GlobalVariable(*M, stringConstant->getType(), false, GlobalValue::PrivateLinkage, stringConstant,"");
				SmallVector<Value*, 4> arg_printf_params;
				arg_printf_params.clear();
				Value *Idxz[2] = {ConstantInt::get(Type::getInt64Ty(ctx), 0),  ConstantInt::get(Type::getInt64Ty(ctx), 0)};
				Value *stringGlobal_ptr = SecondIfBBBuilder.CreateGEP(stringGlobal, Idxz, "");
				Value *arg_1=SecondIfBBBuilder.CreateCast(Instruction::BitCast, stringGlobal_ptr, PointerType::get(Type::getInt8Ty(ctx), 0));
				arg_printf_params.push_back(arg_1);
				arg_printf_params.push_back(idx);
				Value *printf_func_call = SecondIfBBBuilder.CreateCall(printf_f, arg_printf_params, "");
				cast<CallInst>(printf_func_call)->setTailCall();
			}
#endif
			for (Function::arg_iterator IT = new_f->arg_begin(), ITE = new_f->arg_end(); IT != ITE; ++IT, ++ArgNo){
				SmallVector<unsigned, 4>::iterator it = std::find(OutErrorBounds.begin(), OutErrorBounds.end(), ArgNo);
				if (it != OutErrorBounds.end()) { // found element
					Argument *arg = &*IT;

					Value *Idxz[2] = {idx,  ConstantInt::get(Type::getInt32Ty(ctx), next_idx)};
					Value *out_ptr = SecondIfBBBuilder.CreateGEP(data, Idxz, "");
					//Value *out = SecondIfBBBuilder.CreateAlignedLoad(out_ptr, 4, false, "");
					Value *out = SecondIfBBBuilder.CreateLoad(out_ptr, false, "");
					SecondIfBBBuilder.CreateStore(out, arg, false);
					next_idx++;
				}
			}
			SecondIfBBBuilder.CreateBr(ReturnBB);

			/* In this block, you check for the loop condition (idx <= tail)::
			for.inc:                                          ; preds = %land.lhs.true, %if.end, %for.body
			%add = add nsw i32 %idx.088, 1 // add one to tail
			%rem = srem i32 %add, 3 // mod with TABLE_SIZE
			%cmp = icmp eq i32 %rem, %2 // check if idx<=tail
			br i1 %cmp, label %for.end, label %for.body
			*/
			IRBuilder<> LoopIncBBBuilder(LoopIncBB);
			Value *inc_idx = LoopIncBBBuilder.CreateAdd(idx_phi, ConstantInt::get(Type::getInt32Ty(ctx), 1), "", false, true);
			Value *inc_idx_by_table_size = LoopIncBBBuilder.CreateSRem(inc_idx, ConstantInt::get(Type::getInt32Ty(ctx), GLOBAL_TABLE_SIZE), "");
			
			Value *dec = LoopIncBBBuilder.CreateAdd(num_elements_phi, ConstantInt::get(Type::getInt32Ty(ctx), -1), "", false, true);
			Value *cmp_loop = LoopIncBBBuilder.CreateICmpSGT(dec, ConstantInt::get(Type::getInt32Ty(ctx), 0), "");
			/*Value *branch = */LoopIncBBBuilder.CreateCondBr(cmp_loop, LoopBodyBB, LoopExitBB);

			/* Add back the phi instructions second argument */
			idx_phi->addIncoming(inc_idx_by_table_size,LoopIncBB); 
			num_elements_phi->addIncoming(dec,LoopIncBB); 

			/*LoopExitBB:: Now we are done with the loop which did not approximate the function, we should
			call the precise version of the function::
			call void @precise_func(i32 %40, i32 %41, double* %42, double* %43)
			*/

			IRBuilder<> LoopExitBBBuilder(LoopExitBB);

			CallSite CI(OldCallInsn);
			const AttributeSet &OldCallPAL = CI.getAttributes();
			// Add any return attributes.
			SmallVector<AttributeSet, 8> AttrVec;
			if (OldCallPAL.hasAttributes(AttributeSet::ReturnIndex))
				AttrVec.push_back(AttributeSet::get(ctx, OldCallPAL.getRetAttributes()));

			SmallVector<Value*, 16> arg_params;
			arg_params.clear();
			int Index = 1; // TODO verify, if the index should start from 0 or 1
			for (Function::arg_iterator IT = new_f->arg_begin(), ITE = new_f->arg_end(); IT != ITE; ++IT, Index++) {
				arg_params.push_back(IT);
				AttributeSet Attrs = OldCallPAL.getParamAttributes(Index);
				if (Attrs.hasAttributes(Index)) {
					AttrBuilder B(Attrs, Index);
					AttrVec.push_back(AttributeSet::get(ctx, arg_params.size(), B));
				}
			}

			Value *precise_func_call;

			if (InvokeInst *invoke = dyn_cast<InvokeInst>(OldCallInsn)) { /* we do not handle this currently */
				precise_func_call = LoopExitBBBuilder.CreateInvoke(f, invoke->getNormalDest(), invoke->getUnwindDest(), arg_params, "");
				cast<InvokeInst>(precise_func_call)->setCallingConv(CI.getCallingConv());
				cast<InvokeInst>(precise_func_call)->setAttributes(AttributeSet::get(ctx, AttrVec));
			} else {
				precise_func_call = LoopExitBBBuilder.CreateCall(f, arg_params, "");
				cast<CallInst>(precise_func_call)->setCallingConv(CI.getCallingConv());
				cast<CallInst>(precise_func_call)->setAttributes(AttributeSet::get(ctx, AttrVec));
				if (cast<CallInst>(OldCallInsn)->isTailCall()) cast<CallInst>(precise_func_call)->setTailCall();
			}
#ifdef DEBUG_MEMOIZE
			//@.str1 = private unnamed_addr constant [24 x i8] c"Enter into table at=%d\0A\00", align 1
			// %call39 = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([24 x i8]* @.str1, i64 0, i64 0), i32 %2) #8
			if (printf_f != NULL) {
				Constant *stringConstant = ConstantDataArray::getString(ctx, StringRef("Enter into table at=%d\n"), true);
				GlobalValue *stringGlobal =new GlobalVariable(*M, stringConstant->getType(), false, GlobalValue::PrivateLinkage, stringConstant,"");
				SmallVector<Value*, 4> arg_printf_params;
				arg_printf_params.clear();
				Value *Idxz[2] = {ConstantInt::get(Type::getInt64Ty(ctx), 0),  ConstantInt::get(Type::getInt64Ty(ctx), 0)};
				Value *stringGlobal_ptr = LoopExitBBBuilder.CreateGEP(stringGlobal, Idxz, "");
				Value *arg_1=LoopExitBBBuilder.CreateCast(Instruction::BitCast, stringGlobal_ptr, PointerType::get(Type::getInt8Ty(ctx), 0));
				arg_printf_params.push_back(arg_1);
				arg_printf_params.push_back(tail);
				Value *printf_func_call = LoopExitBBBuilder.CreateCall(printf_f, arg_printf_params, "");
				cast<CallInst>(printf_func_call)->setTailCall();
			}
#endif

			/*  %idxprom39 = sext i32 %rem34 to i64 */
			Value *updated_tail = LoopExitBBBuilder.CreateSExt(tail, Type::getInt64Ty(ctx), "");  

			/* store inputs...
			%in142 = getelementptr inbounds %struct.global_struct_s_* %12, i64 %idxprom39, i32 2
			store i32 %in1, i32* %in142, align 4, !tbaa !4
			%in246 = getelementptr inbounds %struct.global_struct_s_* %12, i64 %idxprom39, i32 3
			store i32 %in2, i32* %in246, align 4, !tbaa !4
			*/
			ArgNo = 0;
			next_idx=0;
			for (Function::arg_iterator IT = new_f->arg_begin(), ITE = new_f->arg_end(); IT != ITE; ++IT, ++ArgNo){
				SmallVector<pair<unsigned, float>, 4>::iterator it = std::find_if(InErrorBounds.begin(), InErrorBounds.end(), CompareFirstElement(ArgNo));
				if (it != InErrorBounds.end()) { // found element
					Argument *arg = &*IT;
					Value *Idxz[2] = {updated_tail,  ConstantInt::get(Type::getInt32Ty(ctx), number_of_output_arguments + next_idx)};
					Value *in_ptr = LoopExitBBBuilder.CreateGEP(data, Idxz, "");
					/*StoreInst *si = */LoopExitBBBuilder.CreateStore(arg, in_ptr, false);
					next_idx++;
				}
			}

			/* store outputs:
			%13 = load double* %out1, align 8, !tbaa !0
			%out150 = getelementptr inbounds %struct.global_struct_s_* %12, i64 %idxprom39, i32 0
			store double %13, double* %out150, align 8, !tbaa !0

			%14 = load double* %out2, align 8, !tbaa !0
			%out254 = getelementptr inbounds %struct.global_struct_s_* %12, i64 %idxprom39, i32 1
			store double %14, double* %out254, align 8, !tbaa !0 */

			next_idx = 0; ArgNo = 0;
			for (Function::arg_iterator IT = new_f->arg_begin(), ITE = new_f->arg_end(); IT != ITE; ++IT, ++ArgNo){
				SmallVector<unsigned, 4>::iterator it = std::find(OutErrorBounds.begin(), OutErrorBounds.end(), ArgNo);
				if (it != OutErrorBounds.end()) { // found element
					Argument *arg = &*IT;
					Value *Idxz[2] = {updated_tail,  ConstantInt::get(Type::getInt32Ty(ctx), next_idx)};
					Value *out_ptr = LoopExitBBBuilder.CreateGEP(data, Idxz, "");
					//Value *out = LoopBodyBBBuilder.CreateAlignedLoad(arg, 4, false, "");
					Value *arg_out = LoopExitBBBuilder.CreateLoad(arg, false, "");
					LoopExitBBBuilder.CreateStore(arg_out, out_ptr, false);
					next_idx++;
				}
			}



			/* Adjust the head and tail pointers since we are going to add a nw entry in the table now!!!

			Source:
			tail = (tail + 1) % MAX_TABLE_ENTRY;
			head = (tail == head) ? (tail + 1) % MAX_TABLE_ENTRY : head;

			%add33 = add nsw i32 %2, 1  // add one to tail
			%rem34 = srem i32 %add33, 3 // mod it with TABLE_SIZE  to get the new tail
			%cmp35 = icmp eq i32 %rem34, %1  // if head == tail, then increment head with a mod or else use the old head
			br i1 %cmp35, label %cond.true, label %cond.end
			*/
			Value *inc_tail = LoopExitBBBuilder.CreateAdd(tail, ConstantInt::get(Type::getInt32Ty(ctx), 1), "", false, true);
			Value *inc_tail_by_table_size = LoopExitBBBuilder.CreateSRem(inc_tail, ConstantInt::get(Type::getInt32Ty(ctx), GLOBAL_TABLE_SIZE), "");
			Value *cmpare = LoopExitBBBuilder.CreateICmpEQ(inc_tail_by_table_size, head, "");
			/*Value *branch1 = */LoopExitBBBuilder.CreateCondBr(cmpare, LoopExitCondTrueBB, LoopExitCondFalseBB);

			/* In this basic block:: We know head == tail, so now assign head = (tail+1) % TABLE_SIZE
			cond.true:                                        ; preds = %for.end
			%add37 = add nsw i32 %rem34, 1 // add one to tail
			%rem38 = srem i32 %add37, 3 // mod with TABLE_SIZE
			br label %cond.end
			*/

			IRBuilder<> LoopExitCondTrueBBBuilder(LoopExitCondTrueBB);
			Value *inc_inc_tail = LoopExitCondTrueBBBuilder.CreateAdd(inc_tail_by_table_size, ConstantInt::get(Type::getInt32Ty(ctx), 1), "", false, true);
			Value *inc_inc_tail_by_table_size = LoopExitCondTrueBBBuilder.CreateSRem(inc_inc_tail, ConstantInt::get(Type::getInt32Ty(ctx), GLOBAL_TABLE_SIZE), "");
			/*Value *branch2 = */LoopExitCondTrueBBBuilder.CreateBr(LoopExitCondFalseBB);

			/*In this basic block, we have updated head and tail pointers
			we need to store the ins and outs in the table one by one
			update the table's head and tail with the current head and tail values ::

			cond.end:                                         ; preds = %cond.true, %for.end
			%cond = phi i32 [ %rem38, %cond.true ], [ %1, %for.end ]
			%idxprom39 = sext i32 %rem34 to i64
			%11 = load %struct.global_struct_s** @global_table_per_axc, align 8, !tbaa !3
			%data40 = getelementptr inbounds %struct.global_struct_s* %11, i64 0, i32 2
			%12 = load %struct.global_struct_s_** %data40, align 8, !tbaa !3

			%in142 = getelementptr inbounds %struct.global_struct_s_* %12, i64 %idxprom39, i32 2
			store i32 %in1, i32* %in142, align 4, !tbaa !4
			%in246 = getelementptr inbounds %struct.global_struct_s_* %12, i64 %idxprom39, i32 3
			store i32 %in2, i32* %in246, align 4, !tbaa !4
			%13 = load double* %out1, align 8, !tbaa !0
			%out150 = getelementptr inbounds %struct.global_struct_s_* %12, i64 %idxprom39, i32 0
			store double %13, double* %out150, align 8, !tbaa !0
			%14 = load double* %out2, align 8, !tbaa !0
			%out254 = getelementptr inbounds %struct.global_struct_s_* %12, i64 %idxprom39, i32 1
			store double %14, double* %out254, align 8, !tbaa !0
			%head55 = getelementptr inbounds %struct.global_struct_s* %11, i64 0, i32 0
			store i32 %cond, i32* %head55, align 4, !tbaa !4
			%tail56 = getelementptr inbounds %struct.global_struct_s* %11, i64 0, i32 1
			store i32 %rem34, i32* %tail56, align 4, !tbaa !4
			*/
			IRBuilder<> LoopExitCondFalseBBBuilder(LoopExitCondFalseBB);

			/*  %cond = phi i32 [ %rem38, %cond.true ], [ %1, %for.end ] */
			PHINode *head_phi = LoopExitCondFalseBBBuilder.CreatePHI(Type::getInt32Ty(ctx), 2, "");
			head_phi->addIncoming(inc_inc_tail_by_table_size,LoopExitCondTrueBB);
			head_phi->addIncoming(head,LoopExitBB);

			
			/* %11 = load %struct.global_struct_s** @global_table_per_axc, align 8, !tbaa !3 */
			/* global_table_ptr */

			/*
			%data40 = getelementptr inbounds %struct.global_struct_s* %11, i64 0, i32 2
			%12 = load %struct.global_struct_s_** %data40, align 8, !tbaa !3 */
			/* data */


			/* store the head and tail values to table:
			%head55 = getelementptr inbounds %struct.global_struct_s* %11, i64 0, i32 0 (head_ptr)
			store i32 %cond, i32* %head55, align 4, !tbaa !4
			%tail56 = getelementptr inbounds %struct.global_struct_s* %11, i64 0, i32 1  (tail_ptr)
			store i32 %rem34, i32* %tail56, align 4, !tbaa !4
			*/
			/*StoreInst *head_store =*/ LoopExitCondFalseBBBuilder.CreateStore(head_phi, head_ptr, false);
			/*StoreInst *tail_store =*/ LoopExitCondFalseBBBuilder.CreateStore(inc_tail_by_table_size, tail_ptr, false);


			LoopExitCondFalseBBBuilder.CreateBr(ReturnBB);

			/* return block:
			ret void
			*/
			IRBuilder<> ReturnBBBuilder(ReturnBB);
			ReturnBBBuilder.CreateRetVoid();

			free (ins); 
			free (cmps);
			free(second_cmps);

			return new_f;
}

/* Step 1: Find the call sites that has axc pragma from meta data */
/* Assumption current functions do not return anything -- input and out are scalar args*/
/* Important:: Generate the axc string for the set of functions as a global string */

/* Step 2: using the IN arguments and OUT arguments, find their types and build a global struct */
/* Step 2a: create two counters -- head and tail  for the same table before */
/* Example: struct { int head; int tail; int IN_1, int IN_2, int OUT_1} axc_struct_line_1; */

/* Step 3: in the main method, at entry malloc a fixed size table for each type -- associate that to the call site */

/* Step 4: Generate approximate code:
a) if IN and OUT arguments are not empty, and their error bounds are known
then get the table corresponding to the current call site/LoC
b) Generate code like this:
if (IN_1 is within  error bounds, IN_2 is wihin error bounds, and so on) {
extract output value and store onto the OUT argument;
// ignore the call instruction
} else {
call the call site...
if the total number of items in the table is the max entry size, replace the head entry and update head 
}
//add a phi node for the call return values!
*/
bool AxCTransform::handle_memo_axc_pragma(Module &M) {

	SmallVector<pair<unsigned, float>, 4> InErrorBounds;
	SmallVector<unsigned, 4> OutErrorBounds;
	SmallVector<pair<unsigned, MDNode*>, 4> MDForInst;

	bool changed = false;

#if 0
	/* TODO: handle WinMain too */
	Function *main_f = M.getFunction("main");
	assert(main_f != NULL);
	IRBuilder<> Builder(&*(inst_begin(main_f))); // First instruction in main method
	Builder.SetInsertPoint(&*(inst_begin(main_f)));
#endif

	/* a global counter to track the number of approximate functions generated */
	num_func_generated_so_far = 0;

	/* List of calls to rewrite*/
	SmallVector<pair<Instruction *, IRBuilder <> &>, 16> precise2axcmap;
	precise2axcmap.clear();

	/* Create signatures for fabs and malloc if they are not there */
	create_signatures(M);

	/* iterate over each call site */
	for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I){
		if (!I->isDeclaration()) {
			Function &F = *I;
			for (inst_iterator II = inst_begin(F), IE = inst_end(F); II != IE; ++II) {
				Instruction *call;
				if ((call = dyn_cast<CallInst>(&*II)) || (call = dyn_cast<InvokeInst>(&*II))) {

					/* call instruction */
					Instruction *OldCall = &*II;	
					CallSite CI(OldCall);
					Function *f=CI.getCalledFunction();

					//DEBUG(dbgs() << "Next Call Insn Before Check:: \n");
					//OldCall->dump();
				

					/* if called function can not be found, ignore it */
					if (f == NULL) continue;

					/* we do not handle invoke instructions */
					if (InvokeInst *invoke = dyn_cast<InvokeInst>(OldCall)) continue;

					/* no meta data, ignore it */
					if (!OldCall->hasMetadata()) continue;

					/* we do not memoize non-void functions */
					if (!f->getReturnType()->isVoidTy()) continue;

					/* we do not allow functions with memoize and axc or axc_precision flag on at the same time */
					if (f->hasFnAttribute(Attribute::AxC) || f->hasFnAttribute(Attribute::AxCPrecision)) continue;

					//DEBUG(dbgs() << "Call Insn After Check: \n");
					//OldCall->dump();

					/* Basic test for the first meta data indicating there is axc pragma */
					/* TODO */


					/* Extract all the meta data */
					InErrorBounds.clear();
					OutErrorBounds.clear();

					/* iterate over all meta data and extract error bounds */
					// Raj:: verify the axc_memoize meta data
					// Get the input meta data 
					if (MDNode *AxCINMemos = OldCall->getMetadata(LLVMContext::MD_axc_memoize_in)) {
						unsigned NumOperands = AxCINMemos->getNumOperands();	
						for (unsigned j = 0; j <NumOperands; j++) {
							if (MDNode *AxCINMemo = dyn_cast<MDNode>(AxCINMemos->getOperand(j))) {
								unsigned NumINOperands = AxCINMemo->getNumOperands();
								unsigned NumINAxCErrorBounds = NumINOperands / 2;
								for (unsigned i = 0; i < NumINAxCErrorBounds; ++i) {
									ConstantInt *argNum = dyn_cast<ConstantInt>(AxCINMemo->getOperand(2*i));
									unsigned argNumV = (unsigned)(*(argNum->getValue().getRawData())); 
									assert(argNumV >=0);
									ConstantFP *errorBound = dyn_cast<ConstantFP>(AxCINMemo->getOperand(2*i + 1));
									float errorBoundV = errorBound->getValueAPF().convertToFloat();
									assert(errorBoundV >= 0 && errorBoundV <=100);
									//DEBUG(dbgs() << "Next ErrorBound:: arg=" << argNumV << " errorPercent=" << errorBoundV << "\n");
									InErrorBounds.push_back(pair<unsigned, float>(argNumV, errorBoundV));
								}
							}
						}
					}
					if (MDNode *AxCOUTMemos = OldCall->getMetadata(LLVMContext::MD_axc_memoize_out)) {
						unsigned NumOperands = AxCOUTMemos->getNumOperands();
						for (unsigned j = 0; j < NumOperands; j++) {
							if (MDNode *AxCOUTMemo = dyn_cast<MDNode>(AxCOUTMemos->getOperand(j))) {
								unsigned NumOUTAxCOperands = AxCOUTMemo->getNumOperands();
								for (unsigned i = 0; i < NumOUTAxCOperands; ++i) {
									ConstantInt *argNum = dyn_cast<ConstantInt>(AxCOUTMemo->getOperand(i));
									unsigned argNumV = (unsigned)(*(argNum->getValue().getRawData()));
									assert(argNumV >= 0);
									//DEBUG(dbgs() << "Next Output:: arg=" << argNumV << "\n");
									OutErrorBounds.push_back(argNumV);
								}
							}
						}
					}

					if (InErrorBounds.empty() || OutErrorBounds.empty()) continue;

					/* Create the struct containing the following */
					

					number_of_output_arguments = 0;
					number_of_input_arguments = 0;

					create_global_memoization_table_type_per_axc_pragma(f, InErrorBounds, OutErrorBounds);

					/* do not do anything if it does not have any params */
					if(global_struct_type == NULL || global_struct_type_for_data == NULL || number_of_output_arguments == 0 || number_of_input_arguments == 0) continue;

					//DEBUG(dbgs() << "#output_args=" << number_of_output_arguments << " #input_args=" << number_of_input_arguments << "\n");
					/* create a new global variable of this type */
					GlobalVariable *global_table_per_axc_pragma = new GlobalVariable(M, 
						PointerType::get(global_struct_type, 0),  
						false,
						GlobalVariable::CommonLinkage, 
						0,
						"");
					global_table_per_axc_pragma->setAlignment(TD->getPointerPrefAlignment());
					global_table_per_axc_pragma->setInitializer(ConstantPointerNull::get(PointerType::get(global_struct_type, 0)));

					/* create the initialization function */
					string InitName = "axc_memoize_init";
					InitName += num_func_generated_so_far;
					FunctionType *init_func_ty = FunctionType::get(Type::getVoidTy(M.getContext()), false);
					Function *initFunc = Function::Create(init_func_ty, GlobalValue::PrivateLinkage, InitName.c_str());
					M.getFunctionList().insert(f, initFunc);
					BasicBlock *InitBB = BasicBlock::Create(M.getContext(), "", initFunc);
					InitBB->setName("entry");
					IRBuilder<> Builder(InitBB); // First instruction in main method
					
					/* Step 3: in the main method, at entry malloc a fixed size table for each type -- associate that to the call site */
					allocate_global_memoization_table_per_axc_pragma(M, global_table_per_axc_pragma, Builder);
					/* Return void */
					Builder.CreateRetVoid();

					/* Create a new function for approximate computing*/
					Function *new_f = create_approximate_function(f, OldCall, InErrorBounds, OutErrorBounds, global_table_per_axc_pragma, initFunc);

					/* TODO replace the old call by the new call */
					const AttributeSet &OldCallPAL = CI.getAttributes();
					// Add any return attributes.
					SmallVector<AttributeSet, 8> AttrVec;
					if (OldCallPAL.hasAttributes(AttributeSet::ReturnIndex))
						AttrVec.push_back(AttributeSet::get(M.getContext(), OldCallPAL.getRetAttributes()));

					SmallVector<Value *, 16> params;
					params.clear();
					unsigned Index = 1;
					for (CallSite::arg_iterator AAI = CI.arg_begin(), AAE = CI.arg_end(); AAI != AAE; ++AAI, ++Index) {
						params.push_back(*AAI);          // copy the old arguments
						AttributeSet Attrs = OldCallPAL.getParamAttributes(Index);
						if (Attrs.hasAttributes(Index)) {
							AttrBuilder B(Attrs, Index);
							AttrVec.push_back(AttributeSet::get(M.getContext(), params.size(), B));
						}
					}


					Instruction * NewCall;
					IRBuilder<> CallBuilder(OldCall);
					if (InvokeInst *invoke = dyn_cast<InvokeInst>(OldCall)) { /* we do not handle this currently */
						NewCall = CallBuilder.CreateInvoke(new_f, invoke->getNormalDest(), invoke->getUnwindDest(), params, "");
						cast<InvokeInst>(NewCall)->setCallingConv(CI.getCallingConv());
						cast<InvokeInst>(NewCall)->setAttributes(AttributeSet::get(new_f->getContext(), AttrVec));
					} else {
						NewCall = CallBuilder.CreateCall(new_f, params, "");
						cast<CallInst>(NewCall)->setCallingConv(CI.getCallingConv());
						cast<CallInst>(NewCall)->setAttributes(AttributeSet::get(new_f->getContext(), AttrVec));
						if (cast<CallInst>(OldCall)->isTailCall()) cast<CallInst>(NewCall)->setTailCall();
					}
					precise2axcmap.push_back(pair<Instruction *, IRBuilder<> &>(OldCall, CallBuilder));
					changed |= true;
				}
			}
		}
	}

	for(SmallVector<pair<Instruction *, IRBuilder <> &>, 16>::iterator it = precise2axcmap.begin(), ite = precise2axcmap.end();
		it != ite; ++it) {
			Instruction *OldCall = it->first;
			IRBuilder<> &CallBuilder = it->second;
			CallBuilder.SetInsertPoint(OldCall);
			/* Now Save it*/
			OldCall->eraseFromParent();
	}
	return changed;
}


/* This handles axc pragmas with function definitions */
bool AxCTransform::handle_axc_pragma (Module &M) { 
	string axc_function_annot_string;
	string axc_precision_reduce_function_annot_string;
	
	/* dump to file too */
	ofstream axcfile("axcFunctions.txt", ofstream::app);
	ofstream pxcfile("pxcFunctions.txt", ofstream::app);

	for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I){
		if (!I->isDeclaration()) {
			Function &F = *I;
			if (F.hasFnAttribute(Attribute::AxC)) {
				string new_string(F.getName());
				axcfile << new_string << "\n";
				axc_function_annot_string.append(new_string + " ");
			} else if (F.hasFnAttribute(Attribute::AxCPrecision)) {
				string new_string(F.getName());
				pxcfile << new_string << "\n";
				axc_precision_reduce_function_annot_string.append(new_string + " ");
			}
		}
	}
	axcfile.close();
	pxcfile.close();

	//Generate these strings even if there is nothing in them
	/* Add the axc function list annotation string */
	Constant *annoStr = ConstantDataArray::getString(M.getContext(),  axc_function_annot_string , true);
	//add String annotations to the current module
	GlobalValue *globalv = new GlobalVariable(M, annoStr->getType(), false,
		GlobalValue::WeakODRLinkage, annoStr,
		"__axc_function_list_string__");

	//Generate these strings even if there is nothing in them
	/* Add the axc precision_reduce function list annotation string */
	Constant *annoStr1 = ConstantDataArray::getString(M.getContext(), axc_precision_reduce_function_annot_string , true);
	//add String annotations to the current module
	GlobalValue *globalv1 = new GlobalVariable(M, annoStr1->getType(), false,
		GlobalValue::WeakODRLinkage, annoStr1,
		"__axc_precision_reduce_function_list_string__");
	return true;
}

bool AxCTransform::runOnModule(Module &M) {
	bool changed = false;
	TD = getAnalysisIfAvailable<DataLayout>();
	/* if memoization is enabled, then perform memoization */
	/* else, just create the string and finish it */
	changed |= handle_axc_pragma(M);
	MEMO_FLAG= memo_f;
	if(MEMO_FLAG) {
		GLOBAL_TABLE_SIZE = global_table_size;
		changed |= handle_memo_axc_pragma(M);
	}
	//M.dump();
	return changed;
}


