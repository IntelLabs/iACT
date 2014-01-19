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

//std::ofstream* out = 0;

//#define ProbBitFlips -0.1
//#define ProbBitFlips 1.1

extern float ProbBitFlips;

/* ===================================================================== */
/* Handlers                                                              */
/* ===================================================================== */

EXCEPT_HANDLING_RESULT GlobalHandler(THREADID threadIndex, EXCEPTION_INFO * pExceptInfo, 
                                      PHYSICAL_CONTEXT * pPhysCtxt, VOID *v)
{
    cout << "GlobalHandler: Caught unexpected exception. " << PIN_ExceptionToString(pExceptInfo) << endl;
    return EHR_UNHANDLED;
}


EXCEPT_HANDLING_RESULT DivideHandler(THREADID tid, EXCEPTION_INFO * pExceptInfo, 
                                        PHYSICAL_CONTEXT * pPhysCtxt, VOID *appContextArg)
{
    if(PIN_GetExceptionCode(pExceptInfo) == EXCEPTCODE_INT_DIVIDE_BY_ZERO) 
    {
#if 1
        //Temporary work-around, Remove when Mantis #1986 is resolved
        string str = PIN_ExceptionToString(pExceptInfo);
        printf("GlobalHandler: Caught divide by zero exception. %s\n", str.c_str());
#else
        cout << "GlobalHandler: Caught divide by zero exception. " << PIN_ExceptionToString(pExceptInfo) << endl;
#endif
        CONTEXT * appCtxt = (CONTEXT *)appContextArg;
        ADDRINT faultIp = PIN_GetContextReg(appCtxt, REG_INST_PTR);
        PIN_SetExceptionAddress(pExceptInfo, faultIp);
        PIN_RaiseException(appCtxt, tid, pExceptInfo); //never returns 
    }
    return EHR_CONTINUE_SEARCH;
}

/*inline int GenerateRandomNumberInRange(int low, int high)
{
    //int low = -3, high = 3;                                       
    
    int range = (high - low) + 1;                                 
    int random_number;                                            
    random_number = low + int(range * (rand()/(RAND_MAX + 1.0))); 
    return random_number;

}*/

//inline int GenerateRandomNumberInRange_0to31()
//{
//    
//    int low = 0, high = 31;                                             
//    int range = (high - low) + 1;                                       
//    int random_number;      
//    
//    //cout << "Prob specified " << ProbBitFlips << "\n" << endl;
//    //Generate a random number between 0 and 1; return a valid number only if generated with the required probability
//    
//    float val = rand()/(RAND_MAX + 1.0);
//    
//    if( val <= ProbBitFlips )
//        random_number = low + int(range * val);       
//    else
//        random_number = -1;
//        
//    return random_number;
//
//}
//
//inline int GenerateRandomNumberInRange_0to63()
//{
//    
//    int low = 0, high = 63;                                             
//    int range = (high - low) + 1;                                       
//    int random_number;                                                  
//
//    //cout << "Prob specified " << ProbBitFlips << "\n" << endl;
//    //Generate a random number between 0 and 1; return a valid number only if generated with the required probability
//    
//    float val = rand()/(RAND_MAX + 1.0);
//    
//    if( val <= ProbBitFlips )
//        random_number = low + int(range * val);       
//    else
//        random_number = -1;
//
//    return random_number;
//
//}

/* ===================================================================== */
/* Analysis routine that emulates div                                    */
/* ===================================================================== */
//-------------Divide Instructions--------------------//
VOID EmulateFloatRegDivide(float *reg1Ptr, float *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Div in Reg" << endl;
    //cout << flush;
    PIN_TryStart(tid, DivideHandler, ctxt);
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Div res: " << (*reg1Ptr/(*reg2Ptr)) << endl;
    float temp = (*reg1Ptr);
    *reg1Ptr = temp/(*reg2Ptr);

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);
    
    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
   
    PIN_TryEnd(tid);
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << endl;

}

VOID EmulateFloatMemDivide(float *reg1Ptr, float *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Div in Mem" << endl;
    //cout << flush;
    float divisor = 0;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);

    PIN_TryStart(tid, DivideHandler, ctxt);
     
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Div res: " << (*reg1Ptr/(*reg2Ptr)) */<< endl;
    float temp = (*reg1Ptr);
    *reg1Ptr = temp/divisor;

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);
    
    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
   
    PIN_TryEnd(tid);
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr << endl;

}

VOID EmulateDoubleRegDivide(double *reg1Ptr, double *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Div in Regs" << endl;
    //cout << flush;
    PIN_TryStart(tid, DivideHandler, ctxt);
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Div res: " << (*reg1Ptr/(*reg2Ptr)) << endl;
    double temp = (*reg1Ptr);
    *reg1Ptr = temp/(*reg2Ptr);

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 63 
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    PIN_TryEnd(tid);
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << endl;

}

VOID EmulateDoubleMemDivide(double *reg1Ptr, double *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Div in Mem" << endl;
    //cout << flush;
    double divisor;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);

    PIN_TryStart(tid, DivideHandler, ctxt);
     
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Div res: " << (*reg1Ptr/(*reg2Ptr)) */<< endl;
    double temp = (*reg1Ptr);
    *reg1Ptr = temp/divisor;

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    PIN_TryEnd(tid);
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr << endl;

}

/* ===================================================================== */
/* Analysis routine that emulates mul                                    */
/* ===================================================================== */
//-------------Multiply Instructions--------------------//
VOID EmulateFloatRegMultiply(float *reg1Ptr, float *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Mul in Regs" << endl;
    //cout << flush;
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Mul res: " << (*reg1Ptr * (*reg2Ptr)) << endl;
    float temp = (*reg1Ptr); 
    *reg1Ptr = temp * (*reg2Ptr);

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
   
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << endl;

}

VOID EmulateFloatMemMultiply(float *reg1Ptr, float *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Mul in Mem" << endl;
    //cout << flush;
    float divisor = 0;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);
     
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Mul res: " << (*reg1Ptr * (*reg2Ptr)) */<< endl;
    float temp = (*reg1Ptr);
    *reg1Ptr = temp * divisor;

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
   
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr << endl;

}

VOID EmulateDoubleRegMultiply(double *reg1Ptr, double *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Mul in Regs" << endl;
    //cout << flush;
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Mul res: " << (*reg1Ptr * (*reg2Ptr)) << endl;
    double temp = (*reg1Ptr); 
    *reg1Ptr = temp * (*reg2Ptr);

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 63 
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << endl;

}

VOID EmulateDoubleMemMultiply(double *reg1Ptr, double *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Mul in Mem" << endl;
    //cout << flush;
    double divisor;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);

    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Mul res: " << (*reg1Ptr * (*reg2Ptr)) */<< endl;
    double temp = (*reg1Ptr); 
    *reg1Ptr = temp * divisor;

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr << endl;

}

/* ===================================================================== */
/* Analysis routine that emulates add                                    */
/* ===================================================================== */
//-------------Add Instructions--------------------//
VOID EmulateFloatRegSum(float *reg1Ptr, float *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Sum in Regs" << endl;
    //cout << flush;
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Sum res: " << (*reg1Ptr + (*reg2Ptr)) << endl;
    float temp = (*reg1Ptr); 
    *reg1Ptr = temp + (*reg2Ptr);

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
   
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Sum res: " << (*reg1Ptr + (*reg2Ptr)) << endl;

}

VOID EmulateFloatMemSum(float *reg1Ptr, float *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Sum in Mem" << endl;
    //cout << flush;
    float divisor = 0;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);

    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Sum res: " << (*reg1Ptr + (*reg2Ptr)) */<< endl;
    float temp = (*reg1Ptr); 
    *reg1Ptr = temp + divisor;

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
    
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Sum res: " << (*reg1Ptr + (*reg2Ptr)) */<< endl;

}

VOID EmulateDoubleRegSum(double *reg1Ptr, double *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Sum in Regs" << endl;
    //cout << flush;
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Mul res: " << (*reg1Ptr + (*reg2Ptr)) << endl;
    double temp = (*reg1Ptr); //*2;
    *reg1Ptr = temp + (*reg2Ptr);

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 63 
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Mul res: " << (*reg1Ptr + (*reg2Ptr)) << endl;

}

VOID EmulateDoubleMemSum(double *reg1Ptr, double *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Mul in Mem" << endl;
    //cout << flush;
    double divisor;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);

    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Sum res: " << (*reg1Ptr + (*reg2Ptr)) */<< endl;
    double temp = (*reg1Ptr); 
    *reg1Ptr = temp + divisor;

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 63 
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Sum res: " << (*reg1Ptr + (*reg2Ptr)) */<< endl;

}

/* ===================================================================== */
/* Analysis routine that emulates sub                                    */
/* ===================================================================== */
//-------------Sub Instructions--------------------//
VOID EmulateFloatRegDifference(float *reg1Ptr, float *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Dif in Regs" << endl;
    //cout << flush;
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Dif res: " << (*reg1Ptr - (*reg2Ptr)) << endl;
    float temp = (*reg1Ptr); 
    *reg1Ptr = temp - (*reg2Ptr);

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
   
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Dif res: " << (*reg1Ptr - (*reg2Ptr)) << endl;

}

VOID EmulateFloatMemDifference(float *reg1Ptr, float *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Float Dif in Mem" << endl;
    //cout << flush;
    float divisor = 0;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);
     
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Dif res: " << (*reg1Ptr - (*reg2Ptr)) */<< endl;
    float temp = (*reg1Ptr); 
    *reg1Ptr = temp - divisor;

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
    
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Dif res: " << (*reg1Ptr - (*reg2Ptr)) */<< endl;

}

VOID EmulateDoubleRegDifference(double *reg1Ptr, double *reg2Ptr, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Dif in Regs" << endl;
    //cout << flush;
     
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Dif res: " << (*reg1Ptr - (*reg2Ptr)) << endl;
    double temp = (*reg1Ptr); 
    *reg1Ptr = temp - (*reg2Ptr);

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);

    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 63
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    //cout << " reg1: " << *reg1Ptr << " reg2: " << *reg2Ptr << " Dif res: " << (*reg1Ptr - (*reg2Ptr)) << endl;

}

VOID EmulateDoubleMemDifference(double *reg1Ptr, double *divisorPtr, unsigned int opSize, CONTEXT * ctxt, THREADID tid)
{
    
    //cout << "Emulating Double Dif in Mem" << endl;
    //cout << flush;
    double divisor;
    PIN_SafeCopy(&divisor, divisorPtr, opSize);

    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Dif res: " << (*reg1Ptr - (*reg2Ptr)) */<< endl;
    double temp = (*reg1Ptr); 
    *reg1Ptr = temp - divisor;

    unsigned char *c = reinterpret_cast<unsigned char*>(reg1Ptr);
    
    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 63 
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    //cout << " reg1: " << *reg1Ptr << " mem: " << *divisorPtr /*<< " Dif res: " << (*reg1Ptr - (*reg2Ptr)) */<< endl;

}

/* ===================================================================== */
/* Analysis routine that emulates Load/Store                             */
/* ===================================================================== */
// Move from memory to register
VOID DoLoad_MOVSS_XMM_MEM(float *regPtr, float * addr, REG regno)
{
    
    //cout << "BEFORE: Emulate loading from addr " << addr << " to " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    float value;
    PIN_SafeCopy(&value, addr, sizeof(float));
    //cout << "Load value " << value << endl;


    unsigned char *c = reinterpret_cast<unsigned char*>(&value);
    
    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
    
    *regPtr = value;
    //cout << "AFTER : Emulate loading from addr " << addr << " to " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    //return value;

}

// Move from register to memory
VOID DoLoad_MOVSS_MEM_XMM(float *regPtr, float * addr, REG regno)
{
    
    //cout << "BEFORE: Emulate loading to addr " << addr << " from " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    float value;
    PIN_SafeCopy(&value, regPtr, sizeof(float));

    unsigned char *c = reinterpret_cast<unsigned char*>(&value);
    
    //Add noise to all the lower 17 bits of a float value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    int mask_1bit = 0x01;
    c[2] = c[2] ^ mask_1bit; //+1 bit
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 31
    //    randomNumber = GenerateRandomNumberInRange_0to31();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber - 16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //}
    
    PIN_SafeCopy(addr, &value, sizeof(float));
    //(*addr) = value;
    //cout << "AFTER : Emulate loading to addr " << addr << " from " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    //return value;

}

// Move from memory to register
VOID DoLoad_MOVSD_XMM_MEM(double *regPtr, double * addr, REG regno)
{
    
    //cout << "BEFORE: Emulate loading from addr " << addr << " to " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    double value;
    PIN_SafeCopy(&value, addr, sizeof(double));
    //cout << "Load value " << value << endl;

    unsigned char *c = reinterpret_cast<unsigned char*>(&value);
    
    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 63
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    *regPtr = value;
    //cout << "AFTER : Emulate loading from addr " << addr << " to " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    //return value;

}

// Move from register to memory
VOID DoLoad_MOVSD_MEM_XMM(double *regPtr, double * addr, REG regno)
{
    
    //cout << "BEFORE: Emulate loading to addr " << addr << " from " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    double value;
    PIN_SafeCopy(&value, regPtr, sizeof(double));
    

    unsigned char *c = reinterpret_cast<unsigned char*>(&value);
    
    //Add noise to all the lower 44 bits of a double value
    int mask = 0xFF;
    c[0] = c[0] ^ mask; // 8 bits
    c[1] = c[1] ^ mask; //+8 bits
    c[2] = c[2] ^ mask; //+8 bits
    c[3] = c[3] ^ mask; //+8 bits
    c[4] = c[4] ^ mask; //+8 bits
    int mask_4bits = 0x0F;
    c[5] = c[5] ^ mask_4bits; //+4 bits
    
    //Add 8 bits of noise anywhere
    //int i, randomNumber;
    //for(i=0; i <=7; i++){
    //    //generate a random number between 0 and 63 
    //    randomNumber = GenerateRandomNumberInRange_0to63();
    //    if(randomNumber >=0 && randomNumber <= 7)
    //        c[0] = c[0] ^ (1UL << randomNumber);
    //    else if(randomNumber >=8 && randomNumber <=15)
    //        c[1] = c[1] ^ (1UL << (randomNumber - 8));
    //    else if(randomNumber >=16 && randomNumber <=23)
    //        c[2] = c[2] ^ (1UL << (randomNumber -16));
    //    else if(randomNumber >=24 && randomNumber <=31)
    //        c[3] = c[3] ^ (1UL << (randomNumber - 24));
    //    else if(randomNumber >=32 && randomNumber <=39)
    //        c[4] = c[4] ^ (1UL << (randomNumber - 32));
    //    else if(randomNumber >=40 && randomNumber <=47)
    //        c[5] = c[5] ^ (1UL << (randomNumber - 40));
    //    else if(randomNumber >=48 && randomNumber <=55)
    //        c[6] = c[6] ^ (1UL << (randomNumber - 48));
    //    else if(randomNumber >=56 && randomNumber <=63)
    //        c[7] = c[7] ^ (1UL << (randomNumber - 56));
    //}
    
    PIN_SafeCopy(addr, &value, sizeof(double));
    //(*addr) = value;
    //cout << "AFTER : Emulate loading to addr " << addr << " from " << REG_StringShort(regno) << " current val " << *regPtr << endl;
    //return value;

}

//All info about an instruction
VOID InstructionDecodeFunc(INS ins)
{

    //cout << "Instruction is " << INS_Mnemonic(ins) << endl;
    //cout << flush;
    //------------------//
    xed_decoded_inst_t* xedd = INS_XedDec(ins);
    
    
    xed_iform_enum_t iform = xed_decoded_inst_get_iform_enum(xedd);
    string string_iform = xed_iform_enum_t2str(iform);
    cout << "IFORM is " << string_iform << endl;

    // To print out the gory details uncomment this:
    char buf[2048];
    xed_decoded_inst_dump(xedd, buf, 2048);
    cout << buf << endl;
    
    xed_syntax_enum_t syntax = XED_SYNTAX_INTEL;  // XED_SYNTAX_ATT, XED_SYNTAX_XED
    const UINT32 BUFLEN = 100;
    char buffer[BUFLEN];
    ADDRINT addr = INS_Address(ins);
    BOOL ok = xed_format(syntax, xedd, buffer, BUFLEN, static_cast<UINT64>(addr));
    if (ok)
    {
        cout << setw(sizeof(ADDRINT)*2) 
             << hex << addr << dec << " "  << buffer << endl;
    }
    else
    {
        cout << "disas-error @" << hex << addr << dec << "\n" << endl;
    }
    //------------------//

}

