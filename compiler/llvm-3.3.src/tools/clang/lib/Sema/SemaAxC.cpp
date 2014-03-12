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



#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
// Raj AxC
#include "clang/AST/DeclAxC.h"
#include "clang/AST/StmtCXX.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Initialization.h"
#include "clang/Sema/SemaInternal.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/Scope.h"
#include "clang/Sema/ScopeInfo.h"
#include <stdio.h>
using namespace clang;



void Sema::ActOnAxCDirective(const Token &Identifier,
                      Scope *curScope,
                      SourceLocation PragmaLoc) {
	// Add to the AST here
	AxCOn =  true;

}

void Sema::ActOnAxCPrecisionDirective(const Token &Identifier,
                      Scope *curScope,
                      SourceLocation PragmaLoc) {
	// Add to the AST here
	AxCPrecisionOn = true;
}


// We allow only the following kinds:
// (1) #pragma axc error(1,5) error(2,5) error(3,15)
//        stmt -- must be a call expression statement
// (2) #pragma axc 
//        stmt -- must be a call expression stmt
void Sema::ActOnAxCMemoDirective(const Token &Identifier,
                      Scope *curScope,
                      SourceLocation PragmaLoc, SmallVector<std::pair<Expr *, Expr *>, 16> &ArgErrorBoundsIn, SmallVector<Expr *, 8> &ArgErrorBoundsOut) {


	AxCInfo *AxCContext = new AxCInfo();

	AxCMemoize = NULL;

	for (SmallVector<std::pair<Expr *, Expr *>, 16>::iterator it = ArgErrorBoundsIn.begin(), ite = ArgErrorBoundsIn.end(); it != ite; ++it) {
		Expr *argNumberExpr = static_cast<Expr *>(it->first);
		unsigned argNumber = 0;
		if (argNumberExpr) {
			llvm::APSInt Val;
			if (argNumberExpr->isTypeDependent() ||
				argNumberExpr->isValueDependent() ||
				!argNumberExpr->isIntegerConstantExpr(Val, Context)) {
			  Diag(PragmaLoc, diag::err_axc_not_a_const_expr);
			  return; // Ignore
			}
			argNumber = (unsigned) Val.getZExtValue();
		 } else {
			 Diag(PragmaLoc, diag::err_axc_not_a_const_expr);
			 return; // Ignore
		}

		Expr *errorBoundExpr = static_cast<Expr *>(it->second);
		float errorBound = 0;
		if (errorBoundExpr) {
			llvm::APSInt Val;
			if (errorBoundExpr->isTypeDependent() ||
				errorBoundExpr->isValueDependent() || 
				!errorBoundExpr->isIntegerConstantExpr(Val, Context)) {
			  Diag(PragmaLoc, diag::err_axc_not_a_const_expr);
			  return; // Ignore
			}
			errorBound = (float) Val.getZExtValue();
		} else {
			Diag(PragmaLoc, diag::err_axc_not_a_const_expr);
			return; // Ignore
		}

		/* some more sematics checking */
		if(argNumber <0 || errorBound < 0 ||errorBound > 100) {
			  Diag(PragmaLoc, diag::err_axc_not_a_const_expr);
			  return; // Ignore
		}
		
		//fprintf(stderr, "Next axc memoize inerrorpair arg=%d error=%d..\n", argNumber, errorBound);
		AxCContext->insert(argNumber, errorBound);
	}
	for (SmallVector<Expr *, 8>::iterator it = ArgErrorBoundsOut.begin(), ite = ArgErrorBoundsOut.end(); it != ite; ++it) {
		Expr *argNumberExpr = static_cast<Expr *>((*it));
		unsigned argNumber = 0;
		if (argNumberExpr) {
			llvm::APSInt Val;
			if (argNumberExpr->isTypeDependent() ||
				argNumberExpr->isValueDependent() ||
				!argNumberExpr->isIntegerConstantExpr(Val, Context)) {
			  Diag(PragmaLoc, diag::err_axc_not_a_const_expr);
			  return; // Ignore
			}
			argNumber = (unsigned) Val.getZExtValue();
		} else {
			Diag(PragmaLoc, diag::err_axc_not_a_const_expr);
			return; // Ignore
		}
		if(argNumber < 0) {
			  Diag(PragmaLoc, diag::err_axc_not_a_const_expr);
			  return; // Ignore
		}
		//fprintf(stderr, "Next axc memoize out arg=%d..\n", argNumber);
		AxCContext->insert(argNumber);
	}

	if(!AxCContext->empty()) {
		//fprintf(stderr, "Start Saving AxCMemoizeIn in Sema...\n");
		AxCMemoize = AxCContext;
		//fprintf(stderr, "Saved AxCMemoizeIn in Sema OK...\n");
	}
}