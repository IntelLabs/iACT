/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2011 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/*! @file
 * This tool demonstrates an instruction emulator 
 */

//==============================================================================
//Copyright (c) 2013 Intel Corporation.
//All rights reserved.
//==============================================================================
//
//Developed by:
//
//    Accelerator Architecture Lab, Intel Labs
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
//Contact: Asit K Mishra (asit.k.mishra@intel.com)

#include <stdio.h>
#include "pin.H"
#include "pin_isa.H"
//#include "portability.H"
extern "C" {
#include "xed-interface.h"
}
#include <iostream>
#include <iomanip>
#include <fstream>

#include <cstdlib> 

//Half precision implementation library
#include "half.H"
using half_float::half;

//std::ofstream* out = 0;


/* ===================================================================== */
/* Handlers                                                              */
/* ===================================================================== */

EXCEPT_HANDLING_RESULT ReductionGlobalHandler(THREADID threadIndex, EXCEPTION_INFO * pExceptInfo, 
                                      PHYSICAL_CONTEXT * pPhysCtxt, VOID *v)
{
    cout << "ReductionGlobalHandler: Caught unexpected exception. " << PIN_ExceptionToString(pExceptInfo) << endl;
    return EHR_UNHANDLED;
}


EXCEPT_HANDLING_RESULT ReductionDivideHandler(THREADID tid, EXCEPTION_INFO * pExceptInfo, 
                                        PHYSICAL_CONTEXT * pPhysCtxt, VOID *appContextArg)
{
    if(PIN_GetExceptionCode(pExceptInfo) == EXCEPTCODE_INT_DIVIDE_BY_ZERO) 
    {
#if 1
        //Temporary work-around, Remove when Mantis #1986 is resolved
        string str = PIN_ExceptionToString(pExceptInfo);
        printf("ReductionGlobalHandler: Caught divide by zero exception. %s\n", str.c_str());
#else
        cout << "ReductionGlobalHandler: Caught divide by zero exception. " << PIN_ExceptionToString(pExceptInfo) << endl;
#endif
        CONTEXT * appCtxt = (CONTEXT *)appContextArg;
        ADDRINT faultIp = PIN_GetContextReg(appCtxt, REG_INST_PTR);
        PIN_SetExceptionAddress(pExceptInfo, faultIp);
        PIN_RaiseException(appCtxt, tid, pExceptInfo); //never returns 
    }
    return EHR_CONTINUE_SEARCH;
}


/* ===================================================================== */
/* Analysis routine that emulates div                                    */
/* ===================================================================== */
//-------------Divide Instructions--------------------//
VOID PreRed_EmulateFloatRegDivide(float *reg1Ptr, float *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Div in Reg" << endl;
    //cout << flush;
    PIN_TryStart(tid, ReductionDivideHandler, ctxt);
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Div res: " << (*reg1Ptr/(*reg2Ptr)) << endl;
    half Nr    = (half)(*reg1Ptr);
    half Dr    = (half)(*reg2Ptr);
    float temp = (float)(Nr/Dr);

    *reg1Ptr = temp;
   
    PIN_TryEnd(tid);
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << endl;

}

VOID PreRed_EmulateFloatMemDivide(float *reg1Ptr, float *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Div in Mem" << endl;
    //cout << flush;
    float divisor = 0;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);

    PIN_TryStart(tid, ReductionDivideHandler, ctxt);
     
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Div res: " << (*reg1Ptr/(*reg2Ptr)) */<< endl;
    half Nr = (half)(*reg1Ptr);
    half Dr = (half)divisor;
    float temp = (float)(Nr/Dr);
    *reg1Ptr = temp;
   
    PIN_TryEnd(tid);
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr << endl;

}

VOID PreRed_EmulateDoubleRegDivide(double *reg1Ptr, double *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Div in Regs" << endl;
    //cout << flush;
    PIN_TryStart(tid, ReductionDivideHandler, ctxt);
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Div res: " << (*reg1Ptr/(*reg2Ptr)) << endl;
    half Nr    = (half)(*reg1Ptr);
    half Dr    = (half)(*reg2Ptr);
    double temp = (double)(Nr/Dr);
    
    *reg1Ptr = temp;
    
    PIN_TryEnd(tid);
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << endl;

}


VOID PreRed_EmulateDoubleMemDivide(double *reg1Ptr, double *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Div in Mem" << endl;
    //cout << flush;
    double divisor;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);

    PIN_TryStart(tid, ReductionDivideHandler, ctxt);
     
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Div res: " << (*reg1Ptr/(*reg2Ptr)) */<< endl;
    half Nr = (half)(*reg1Ptr);
    half Dr = (half)divisor;
    double temp = (double)(Nr/Dr);
    *reg1Ptr = temp;
    
    PIN_TryEnd(tid);
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr << endl;

}

/* ===================================================================== */
/* Analysis routine that emulates mul                                    */
/* ===================================================================== */
//-------------Multiply Instructions--------------------//
VOID PreRed_EmulateFloatRegMultiply(float *reg1Ptr, float *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Float Mul in Regs" << endl;
   // //cout << flush;
   //  
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Mul res: " << (*reg1Ptr * (*reg2Ptr)) << endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (float)( temp * (half)(*reg2Ptr) );
   //
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << endl;

}

VOID PreRed_EmulateFloatMemMultiply(float *reg1Ptr, float *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Float Mul in Mem" << endl;
   // //cout << flush;
   // float divisor = 0;
   // PIN_SafeCopy(&divisor, divisorPtr, opSize);
   //  
   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Mul res: " << (*reg1Ptr * (*reg2Ptr)) */<< endl;
   // half temp = (half)(*reg1Ptr);
   // *reg1Ptr = (float)( temp * (half)divisor );
   //
   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr << endl;

}

VOID PreRed_EmulateDoubleRegMultiply(double *reg1Ptr, double *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Double Mul in Regs" << endl;
   // //cout << flush;
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Mul res: " << (*reg1Ptr * (*reg2Ptr)) << endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (double)( temp * (half)(*reg2Ptr) );
   // 
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << endl;

}

VOID PreRed_EmulateDoubleMemMultiply(double *reg1Ptr, double *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Double Mul in Mem" << endl;
   // //cout << flush;
   // double divisor;
   // PIN_SafeCopy(&divisor, divisorPtr, opSize);

   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Mul res: " << (*reg1Ptr * (*reg2Ptr)) */<< endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (double)( temp * (half)divisor );
   // 
   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr << endl;

}

/* ===================================================================== */
/* Analysis routine that emulates add                                    */
/* ===================================================================== */
//-------------Add Instructions--------------------//
VOID PreRed_EmulateFloatRegSum(float *reg1Ptr, float *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Float Sum in Regs" << endl;
   // //cout << flush;
   //  
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Sum res: " << (*reg1Ptr + (*reg2Ptr)) << endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (float)( temp + (half)(*reg2Ptr) );
   //
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Sum res: " << (*reg1Ptr + (*reg2Ptr)) << endl;

}

VOID PreRed_EmulateFloatMemSum(float *reg1Ptr, float *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Float Sum in Mem" << endl;
   // //cout << flush;
   // float divisor = 0;
   // PIN_SafeCopy(&divisor, divisorPtr, opSize);

   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Sum res: " << (*reg1Ptr + (*reg2Ptr)) */<< endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (float)( temp + (half)divisor );
   // 
   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Sum res: " << (*reg1Ptr + (*reg2Ptr)) */<< endl;

}

VOID PreRed_EmulateDoubleRegSum(double *reg1Ptr, double *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Double Sum in Regs" << endl;
   // //cout << flush;
   //  
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Mul res: " << (*reg1Ptr + (*reg2Ptr)) << endl;
   // half temp = (half)(*reg1Ptr); //*2;
   // *reg1Ptr = (double)( temp + (half)(*reg2Ptr) );
   // 
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Mul res: " << (*reg1Ptr + (*reg2Ptr)) << endl;

}

VOID PreRed_EmulateDoubleMemSum(double *reg1Ptr, double *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Double Mul in Mem" << endl;
   // //cout << flush;
   // double divisor;
   // PIN_SafeCopy(&divisor, divisorPtr, opSize);

   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Sum res: " << (*reg1Ptr + (*reg2Ptr)) */<< endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (double)( temp + (half)divisor );
   // 
   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Sum res: " << (*reg1Ptr + (*reg2Ptr)) */<< endl;

}

/* ===================================================================== */
/* Analysis routine that emulates sub                                    */
/* ===================================================================== */
//-------------Sub Instructions--------------------//
VOID PreRed_EmulateFloatRegDifference(float *reg1Ptr, float *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Float Dif in Regs" << endl;
   // //cout << flush;
   //  
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Dif res: " << (*reg1Ptr - (*reg2Ptr)) << endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (float)( temp - (half)(*reg2Ptr) );
   //
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Dif res: " << (*reg1Ptr - (*reg2Ptr)) << endl;

}

VOID PreRed_EmulateFloatMemDifference(float *reg1Ptr, float *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Float Dif in Mem" << endl;
   // //cout << flush;
   // float divisor = 0;
   // PIN_SafeCopy(&divisor, divisorPtr, opSize);
   //  
   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Dif res: " << (*reg1Ptr - (*reg2Ptr)) */<< endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (float)( temp - (half)divisor );
   // 
   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Dif res: " << (*reg1Ptr - (*reg2Ptr)) */<< endl;

}

VOID PreRed_EmulateDoubleRegDifference(double *reg1Ptr, double *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Double Dif in Regs" << endl;
   // //cout << flush;
   //  
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Dif res: " << (*reg1Ptr - (*reg2Ptr)) << endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (double)( temp - (half)(*reg2Ptr) );
   // 
   // //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Dif res: " << (*reg1Ptr - (*reg2Ptr)) << endl;

}

VOID PreRed_EmulateDoubleMemDifference(double *reg1Ptr, double *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
   // //cout << "Emulating Double Dif in Mem" << endl;
   // //cout << flush;
   // double divisor;
   // PIN_SafeCopy(&divisor, divisorPtr, opSize);

   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Dif res: " << (*reg1Ptr - (*reg2Ptr)) */<< endl;
   // half temp = (half)(*reg1Ptr); 
   // *reg1Ptr = (double)( temp - (half)divisor );
   // 
   // //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Dif res: " << (*reg1Ptr - (*reg2Ptr)) */<< endl;

}

/* ===================================================================== */
/* Analysis routine that emulates Load/Store                             */
/* ===================================================================== */

// Move from memory to register
VOID PreRed_DoLoad_MOVSS_XMM_MEM(float *regPtr, float * addr, REG regno)
{
/*    
    //cout << "BEFORE: Emulate loading from addr " << addr << " to " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    float value;
    PIN_SafeCopy(&value, addr, sizeof(float));
    //cout << "Load value " << value << endl;
    
    half temp = (half)value;
    *regPtr = (float)temp;
    //cout << "AFTER : Emulate loading from addr " << addr << " to " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    //return value;
*/
}

// Move from register to memory
VOID PreRed_DoLoad_MOVSS_MEM_XMM(float *regPtr, float * addr, REG regno)
{
/*    
    //cout << "BEFORE: Emulate loading to addr " << addr << " from " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    float value;
    PIN_SafeCopy(&value, regPtr, sizeof(float));
   
    half temp = (half)value;
    float retvalue = (float)temp;

    PIN_SafeCopy(addr, &retvalue, sizeof(float));
    //(*addr) = retvalue;
    //cout << "AFTER : Emulate loading to addr " << addr << " from " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    //return retvalue;
*/
}

// Move from memory to register
VOID PreRed_DoLoad_MOVSD_XMM_MEM(double *regPtr, double * addr, REG regno)
{
/*    
    //cout << "BEFORE: Emulate loading from addr " << addr << " to " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    double value;
    PIN_SafeCopy(&value, addr, sizeof(double));
    //cout << "Load value " << value << endl;
    
    half temp = (half)value;
    *regPtr = (double)temp;
    //cout << "AFTER : Emulate loading from addr " << addr << " to " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    //return value;
*/
}

// Move from register to memory
VOID PreRed_DoLoad_MOVSD_MEM_XMM(double *regPtr, double * addr, REG regno)
{
/*    
    //cout << "BEFORE: Emulate loading to addr " << addr << " from " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    double value;
    PIN_SafeCopy(&value, regPtr, sizeof(double));
   
    half temp = (half)value;
    double retvalue = (double)temp;

    PIN_SafeCopy(addr, &retvalue, sizeof(double));
    //(*addr) = retvalue;
    //cout << "AFTER : Emulate loading to addr " << addr << " from " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    //return retvalue;
*/
}

