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
#include "clang/Parse/Parser.h"
#include "clang/Parse/ParseDiagnostic.h"
#include "RAIIObjectsForParser.h"
#include <stdio.h>
using namespace clang;


//===----------------------------------------------------------------------===//
// AxC directives.
//===----------------------------------------------------------------------===//
void Parser::ParseAxCDirective() {
	assert(Tok.is(tok::annot_pragma_axc) && "Not an axc directive!");
	//fprintf(stderr, "invoke ParseAxCDirective\n");
	// Consume axc
	SourceLocation PragmaLoc = ConsumeToken();
	// Consume final annot_pragma_axc_end.
	ExpectAndConsume(tok::annot_pragma_axc_end, diag::err_pragma_axc_illformed, "unexpected characters in pragma definition");
	Actions.ActOnAxCDirective(Tok, getCurScope(), PragmaLoc);
}


//===----------------------------------------------------------------------===//
// AxC Memo directives.
//===----------------------------------------------------------------------===//
void Parser::ParseAxCMemoDirective() {
	assert(Tok.is(tok::annot_pragma_axc_memo) && "Not an AxC Memo directive!");
	//fprintf(stderr, "invoke ParseAxCMemoDirective\n");
	// Consume axc_memo
	SourceLocation PragmaLoc = ConsumeToken();

	SmallVector<std::pair<Expr *, Expr *>, 16> ArgErrorBoundsIn;
	SmallVector<Expr *, 8> ArgErrorBoundsOut;

	// IN [ (expr : expr ), (expr : expr), ... ]
	// IN [ (expr : expr ), (expr : expr), ... ]
	// TODO IN is right now not parsed
	ExpectAndConsume(tok::l_square, diag::err_pragma_axc_memo_illformed,"'[' expected");
	ExpectAndConsume(tok::l_paren, diag::err_pragma_axc_memo_illformed, "'(' expected");
	ExprResult argNumberExpr(ParseExpression());
	/*if (!Tok.isNot(tok::numeric_constant)) {
		PP.Diag(Tok.getLocation(), diag::err_pragma_axc_memo_illformed) << "integer expected";
		return;
	}
	ExprResult argNumberExpr = Actions.ActOnNumericConstant(Tok);*/
	if (argNumberExpr.isInvalid()) return;

	ExpectAndConsume(tok::colon, diag::err_pragma_axc_memo_illformed, "':' expected");
	ExprResult errorPercentageExpr(ParseExpression().get());
	/*if (!Tok.isNot(tok::numeric_constant)) {
		PP.Diag(Tok.getLocation(), diag::err_pragma_axc_memo_illformed) << "floating point value expected";
		return;
	}
	ExprResult errorPercentageExpr = Actions.ActOnNumericConstant(Tok);*/
	if (errorPercentageExpr.isInvalid()) return;
	ExpectAndConsume(tok::r_paren, diag::err_pragma_axc_memo_illformed, "')' expected");
	ArgErrorBoundsIn.push_back(std::pair<Expr *, Expr *>(argNumberExpr.get(), errorPercentageExpr.get()));

	while(Tok.isNot(tok::r_square)) {
		ExpectAndConsume(tok::comma, diag::err_pragma_axc_memo_illformed, "',' expected");
		ExpectAndConsume(tok::l_paren, diag::err_pragma_axc_memo_illformed, "'(' expected");

		ExprResult argNumberExpr1(ParseExpression());
		/*if (!Tok.isNot(tok::numeric_constant)) {
			PP.Diag(Tok.getLocation(), diag::err_pragma_axc_memo_illformed) << "integer expected";
			return;
		}
		ExprResult argNumberExpr1 = Actions.ActOnNumericConstant(Tok);*/
		if (argNumberExpr1.isInvalid()) return;

		ExpectAndConsume(tok::colon, diag::err_pragma_axc_memo_illformed, "':' expected");
		ExprResult errorPercentageExpr1(ParseExpression());
		/*if (!Tok.isNot(tok::numeric_constant)) {
			PP.Diag(Tok.getLocation(), diag::err_pragma_axc_memo_illformed) << "floating point value expected";
			return;
		}
		ExprResult errorPercentageExpr1 = Actions.ActOnNumericConstant(Tok);*/
		if (errorPercentageExpr1.isInvalid()) return;
		
		ExpectAndConsume(tok::r_paren, diag::err_pragma_axc_memo_illformed, "')' expected");
		ArgErrorBoundsIn.push_back(std::pair<Expr *, Expr *>(argNumberExpr1.get(), errorPercentageExpr1.get()));
	}
	ExpectAndConsume(tok::r_square, diag::err_pragma_axc_memo_illformed,"']' expected");

	// Now parse the output arguments
	// Specified as {expr,expr}
	ExpectAndConsume(tok::l_brace, diag::err_pragma_axc_memo_illformed, "'{' expected");
	ExprResult outArgNumExpr(ParseExpression());
	/*if (!Tok.isNot(tok::numeric_constant)) {
		PP.Diag(Tok.getLocation(), diag::err_pragma_axc_memo_illformed) << "integer expected";
		return;
	}
	ExprResult outArgNumExpr = Actions.ActOnNumericConstant(Tok);*/
	if (outArgNumExpr.isInvalid()) return;

	ArgErrorBoundsOut.push_back(outArgNumExpr.get());
	while(Tok.isNot(tok::r_brace)) {
		ExpectAndConsume(tok::comma, diag::err_pragma_axc_memo_illformed, "',' expected");
		ExprResult outArgNumExpr1(ParseExpression());
		/*if (!Tok.isNot(tok::numeric_constant)) {
			PP.Diag(Tok.getLocation(), diag::err_pragma_axc_memo_illformed) << "integer expected";
			return;
		}
		ExprResult outArgNumExpr1 = Actions.ActOnNumericConstant(Tok);*/
		if (outArgNumExpr1.isInvalid()) return;
		ArgErrorBoundsOut.push_back(outArgNumExpr1.get());
	}
	ExpectAndConsume(tok::r_brace, diag::err_pragma_axc_memo_illformed, "'}' expected");

	ExpectAndConsume(tok::annot_pragma_axc_memo_end, diag::err_pragma_axc_memo_illformed, "unexpected characters in pragma definition");
	Actions.ActOnAxCMemoDirective(Tok, getCurScope(), PragmaLoc, ArgErrorBoundsIn, ArgErrorBoundsOut);
}
//===----------------------------------------------------------------------===//
// AxC Precision directives.
//===----------------------------------------------------------------------===//
void Parser::ParseAxCPrecisionDirective() {
	assert(Tok.is(tok::annot_pragma_axc_precision) && "Not an axc_precision directive!");
	//fprintf(stderr, "invoke ParseAxCPrecisionDirective\n");
	// Consume axc
	SourceLocation PragmaLoc = ConsumeToken();
	// Consume final annot_pragma_axc_precisionend.
	ExpectAndConsume(tok::annot_pragma_axc_precision_end, diag::err_pragma_axc_pecision_illformed, "unexpected characters in pragma definition");
	Actions.ActOnAxCPrecisionDirective(Tok, getCurScope(), PragmaLoc);
}

