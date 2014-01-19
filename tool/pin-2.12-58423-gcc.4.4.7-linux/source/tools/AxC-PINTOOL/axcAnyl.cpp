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
//
// @ORIGINAL_AUTHOR: Artur Klauser
// @MODIFIED BY: Asit Mishra (asit.k.mishra@intel.com)
// FIRST MODIFIED: Dec, 13, 2012
//

/*! @file
 *  This file contains an ISA-portable cache simulator
 *  data cache hierarchies (precise and axc)
 *  and function analysis tools
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

// PIN stuffs
#include "pin.H"
#include "pin_profile.H"

// C++ stuffs
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <string.h>
#include <set>

// Implements the cache structures
#include "cache.H"

// Implements the approximate (floating-point) division, multiplication, addition and subtration instructions
// and load/store instructions
#include "emuins.H"

// Implements the half-precision (floating-point) division, multiplication, addition and subtration instructions
// and load/store instructions
#include "reducins.H"

float ProbBitFlips; //defined in emuins.H

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

std::ofstream outFile;

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,    "pintool",
    "o", "profile_axc.out", "specify output file name");
KNOB<string> KnobInputAxCFile(KNOB_MODE_WRITEONCE, "pintool",
    "axc", "functionListAxC.txt", "specify the functions to support via AxC");
KNOB<string> KnobInputPxCFile(KNOB_MODE_WRITEONCE, "pintool",
    "pxc", "functionListPxC.txt", "specify the functions to where precision of variables can be reduced");
KNOB<BOOL> KnobVanillaSimulationMode(KNOB_MODE_WRITEONCE, "pintool",
    "vanilla", "0", "model vanilla system with no AxC");

KNOB<UINT32> KnobDL1CacheSize(KNOB_MODE_WRITEONCE, "pintool",
    "d1c","16", "cache size in kilobytes");
KNOB<UINT32> KnobDL1LineSize(KNOB_MODE_WRITEONCE, "pintool",
    "d1b","32", "cache block size in bytes");
KNOB<UINT32> KnobDL1Associativity(KNOB_MODE_WRITEONCE, "pintool",
    "d1a","8", "cache associativity (1 for direct mapped)");

KNOB<UINT32> KnobAxCCacheSize(KNOB_MODE_WRITEONCE, "pintool",
    "axc_c","1", "cache size in kilobytes");
KNOB<UINT32> KnobAxCLineSize(KNOB_MODE_WRITEONCE, "pintool",
    "axc_b","32", "cache block size in bytes");
KNOB<UINT32> KnobAxCAssociativity(KNOB_MODE_WRITEONCE, "pintool",
    "axc_a","8", "cache associativity (1 for direct mapped)");

KNOB<UINT32> KnobUL2CacheSize(KNOB_MODE_WRITEONCE, "pintool",
    "u2c","32", "cache size in kilobytes");
KNOB<UINT32> KnobUL2LineSize(KNOB_MODE_WRITEONCE, "pintool",
    "u2b","32", "cache block size in bytes");
KNOB<UINT32> KnobUL2Associativity(KNOB_MODE_WRITEONCE, "pintool",
    "u2a","16", "cache associativity (1 for direct mapped)");

KNOB<float> KnobProbabilityBitFlips(KNOB_MODE_WRITEONCE, "pintool",
    "p","1.0", "probability of bit flips (range: 0.0 (never) to 1.0 (always))");
/* ===================================================================== */

INT32 Usage()
{

   cerr <<
        "This tool represents a modified cache simulator for AxC and a function profiler.\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;

}

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

typedef struct RtnCount
{

    string  _name;
    string  _image;
    ADDRINT _address;
    RTN     _rtn;
    UINT64  _rtnCount;
    UINT64  _rtnL1AccessCount;
    UINT64  _rtnL1MissCount;
    UINT64  _rtnL2AccessCount;
    UINT64  _rtnL2MissCount;
    UINT64  _icount;
    bool    _RTNisAxC;
    bool    _RTNisPxC;
    struct RtnCount * _next;
} RTN_COUNT;

// Linked list of instruction counts for each routine
RTN_COUNT * RtnList = 0;

// Set that holds the list of AxC functions specified by a user
std::set<string> mAxCFunctionList;

// Set that holds the list of functions where variable precision is to be reduced (specified by a user)
std::set<string> mPxCFunctionList;

/* ===================================================================== */
/* Cache structures */
/* ===================================================================== */
// wrap configuation constants into their own name space to avoid name clashes
namespace DL1
{
    
    const UINT32 max_sets = 16 * KILO; // cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = 64; // associativity;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;
    //typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
    typedef CACHE_LRU(max_sets, max_associativity, allocation) CACHE;

}

DL1::CACHE* dl1 = NULL;

namespace D_AxC
{

    const UINT32 max_sets = 16 * KILO; // cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = 64; // associativity;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;
    //typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
    typedef CACHE_LRU(max_sets, max_associativity, allocation) CACHE;

}

D_AxC::CACHE* d_axc = NULL;

namespace UL2
{

    const UINT32 max_sets = 64 * KILO; // cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = 64; // associativity;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;
    //typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
    typedef CACHE_LRU(max_sets, max_associativity, allocation) CACHE;

}

UL2::CACHE* ul2 = NULL;

/* ===================================================================== */
/* Function Definitions */
/* ===================================================================== */

/* ===================================================================== */
/* Following four are function implementations of 
 * load/stores for MainCache */
/* ===================================================================== */

/* ===================================================================== */

VOID LoadMultiFast(ADDRINT addr, UINT32 size, UINT64* L1AccessCount, UINT64* L1MissCount, UINT64* L2AccessCount, UINT64* L2MissCount)
{

    if( dl1->Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD, L1AccessCount, L1MissCount) == false )
    {
        ul2->Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD, L2AccessCount, L2MissCount);
    }

}

/* ===================================================================== */

VOID StoreMultiFast(ADDRINT addr, UINT32 size, UINT64* L1AccessCount, UINT64* L1MissCount, UINT64* L2AccessCount, UINT64* L2MissCount)
{

    if( dl1->Access(addr, size, CACHE_BASE::ACCESS_TYPE_STORE, L1AccessCount, L1MissCount) == false )
    {
        ul2->Access(addr, size, CACHE_BASE::ACCESS_TYPE_STORE, L1AccessCount, L1MissCount);
    }

}

/* ===================================================================== */

VOID LoadSingleFast(ADDRINT addr, UINT64* L1AccessCount, UINT64* L1MissCount, UINT64* L2AccessCount, UINT64* L2MissCount)
{
    
    if( dl1->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD, L1AccessCount, L1MissCount) == false )
    {
        ul2->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD, L2AccessCount, L2MissCount);
    }

}

/* ===================================================================== */

VOID StoreSingleFast(ADDRINT addr, UINT64* L1AccessCount, UINT64* L1MissCount, UINT64* L2AccessCount, UINT64* L2MissCount)
{
    
    if( dl1->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_STORE, L1AccessCount, L1MissCount) == false )
    {
        ul2->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_STORE, L2AccessCount, L2MissCount);
    }

}

/* ===================================================================== */

VOID LoadMultiFast_AxC(ADDRINT addr, UINT32 size, UINT64* L1AccessCount, UINT64* L1MissCount, UINT64* L2AccessCount, UINT64* L2MissCount)
{
    
    if( d_axc->Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD, L1AccessCount, L1MissCount) == false )
    {
        ul2->Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD, L2AccessCount, L2MissCount);
    }

}

/* ===================================================================== */
/* Following four are function implementations of 
 * load/stores for AxCCache */
/* ===================================================================== */

/* ===================================================================== */

VOID StoreMultiFast_AxC(ADDRINT addr, UINT32 size, UINT64* L1AccessCount, UINT64* L1MissCount, UINT64* L2AccessCount, UINT64* L2MissCount)
{
    
    if( d_axc->Access(addr, size, CACHE_BASE::ACCESS_TYPE_STORE, L1AccessCount, L1MissCount) == false )
    {
        ul2->Access(addr, size, CACHE_BASE::ACCESS_TYPE_STORE, L1AccessCount, L1MissCount);
    }

}

/* ===================================================================== */

VOID LoadSingleFast_AxC(ADDRINT addr, UINT64* L1AccessCount, UINT64* L1MissCount, UINT64* L2AccessCount, UINT64* L2MissCount)
{
    
    if( d_axc->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD, L1AccessCount, L1MissCount) == false )
    {
        ul2->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD, L2AccessCount, L2MissCount);
    }

}

/* ===================================================================== */

VOID StoreSingleFast_AxC(ADDRINT addr, UINT64* L1AccessCount, UINT64* L1MissCount, UINT64* L2AccessCount, UINT64* L2MissCount)
{
    
    if( d_axc->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_STORE, L1AccessCount, L1MissCount) == false )
    {
        ul2->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_STORE, L2AccessCount, L2MissCount);
    }

}
    
/* ===================================================================== */
/* Instrument noise in function instructions                             */   
/* ===================================================================== */

/* ===================================================================== */
    
VOID Noise_InstrumentArithmeticFunction(INS ins)
{
    
    //InstructionDecodeFunc(ins);
    
    //-----------------------------------------//
    //------------START DIV TESTS--------------//
    //-----------------------------------------//
    //------------Float Div in REGS------------//
    if ((INS_Opcode(ins) == XED_ICLASS_DIVSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_DIVSS_XMMss_XMMss)
       )
        //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(EmulateFloatRegDivide),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Float Div in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_DIVSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_DIVSS_XMMss_MEMss) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(EmulateFloatMemDivide),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //------------Double Div in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_DIVSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_DIVSD_XMMsd_XMMsd)
       )
       //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(EmulateDoubleRegDivide),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Double Div in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_DIVSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_DIVSD_XMMsd_MEMsd) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(EmulateDoubleMemDivide),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //-----------------------------------------//
    //------------END DIV TESTS--------------//
    //-----------------------------------------//
    //-----------------------------------------//
    //------------START MUL TESTS--------------//
    //-----------------------------------------//
    //------------Float Mul in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_MULSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MULSS_XMMss_XMMss)
       )
        //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(EmulateFloatRegMultiply),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Float Mul in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_MULSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MULSS_XMMss_MEMss) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(EmulateFloatMemMultiply),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //------------Double Mul in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_MULSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MULSD_XMMsd_XMMsd)
       )
       //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(EmulateDoubleRegMultiply),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Double Mul in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_MULSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MULSD_XMMsd_MEMsd) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(EmulateDoubleMemMultiply),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //-----------------------------------------//
    //------------END MUL TESTS--------------//
    //-----------------------------------------//
    //-----------------------------------------//
    //------------START ADD TESTS--------------//
    //-----------------------------------------//
    //------------Float Add in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_ADDSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_ADDSS_XMMss_XMMss)
       )
        //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(EmulateFloatRegSum),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Float Add in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_ADDSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_ADDSS_XMMss_MEMss) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(EmulateFloatMemSum),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //------------Double Add in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_ADDSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_ADDSD_XMMsd_XMMsd)
       )
       //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(EmulateDoubleRegSum),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Double Add in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_ADDSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_ADDSD_XMMsd_MEMsd) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(EmulateDoubleMemSum),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //-----------------------------------------//
    //------------END ADD TESTS--------------//
    //-----------------------------------------//
    //-----------------------------------------//
    //------------START SUB TESTS--------------//
    //-----------------------------------------//
    //------------Float Sub in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_SUBSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_SUBSS_XMMss_XMMss)
       )
        //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(EmulateFloatRegDifference),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Float Sub in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_SUBSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_SUBSS_XMMss_MEMss) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(EmulateFloatMemDifference),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //------------Double Sub in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_SUBSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_SUBSD_XMMsd_XMMsd)
       )
       //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(EmulateDoubleRegDifference),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Double Sub in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_SUBSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_SUBSD_XMMsd_MEMsd) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(EmulateDoubleMemDifference),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }

    /*
    if( (INS_Mnemonic(ins) == "FDIV")         // &&
        //(
        //  (INS_Opcode(ins) != XED_ICLASS_IDIV)  ||
        //  (INS_Opcode(ins) != XED_ICLASS_DIVSS) ||
        //  (INS_Opcode(ins) != XED_ICLASS_DIVSD)
        //)
      )
    {
        //InstructionDecodeFunc(ins);
    }
    */
    
    //-----------------------------------------//
    //------------END SUB TESTS--------------//
    //-----------------------------------------//
    //------------START AxC LD/ST TESTS--------------//
    //-----------------------------------------//
    
    //Load floats
    //Floating point (float) move from memory to register
    if ((INS_Opcode(ins) == XED_ICLASS_MOVSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MOVSS_XMMdq_MEMss)
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       AFUNPTR(DoLoad_MOVSS_XMM_MEM),
                       IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                       IARG_MEMORYREAD_EA,
                       IARG_UINT32, REG(INS_OperandReg(ins, 0)),
                       //IARG_RETURN_REGS, INS_OperandReg(ins, 0),
                       IARG_END);
        INS_Delete(ins);
    }

    //Store floats
    //Floating point (float) move from register to memory
    if ((INS_Opcode(ins) == XED_ICLASS_MOVSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MOVSS_MEMss_XMMss)
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       AFUNPTR(DoLoad_MOVSS_MEM_XMM),
                       IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                       IARG_MEMORYWRITE_EA,
                       IARG_UINT32, REG(INS_OperandReg(ins,1)),
                       //IARG_RETURN_REGS, INS_OperandReg(ins, 0),
                       IARG_END);
        INS_Delete(ins);
    }
    
    //Load doubles
    //Floating point (double) move from memory to register
    if ((INS_Opcode(ins) == XED_ICLASS_MOVSD_XMM) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MOVSD_XMM_XMMdq_MEMsd)
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       AFUNPTR(DoLoad_MOVSD_XMM_MEM),
                       IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                       IARG_MEMORYREAD_EA,
                       IARG_UINT32, REG(INS_OperandReg(ins, 0)),
                       //IARG_RETURN_REGS, INS_OperandReg(ins, 0),
                       IARG_END);
        INS_Delete(ins);
    }

    //Store doubles
    //Floating point (double) move from register to memory
    if ((INS_Opcode(ins) == XED_ICLASS_MOVSD_XMM) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MOVSD_XMM_MEMsd_XMMsd)
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       AFUNPTR(DoLoad_MOVSD_MEM_XMM),
                       IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                       IARG_MEMORYWRITE_EA,
                       IARG_UINT32, REG(INS_OperandReg(ins,1)),
                       //IARG_RETURN_REGS, INS_OperandReg(ins, 0),
                       IARG_END);
        INS_Delete(ins);
    }

}

/* ===================================================================== */
/* Instrument Precision Reduction in function instructions               */   
/* ===================================================================== */

/* ===================================================================== */

VOID PrecisionReduction_InstrumentArithmeticFunction(INS ins)
{
    
    //InstructionDecodeFunc(ins);
    
    //-----------------------------------------//
    //------------START DIV TESTS--------------//
    //-----------------------------------------//
    //------------Float Div in REGS------------//
    if ((INS_Opcode(ins) == XED_ICLASS_DIVSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_DIVSS_XMMss_XMMss)
       )
        //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(PreRed_EmulateFloatRegDivide),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Float Div in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_DIVSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_DIVSS_XMMss_MEMss) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(PreRed_EmulateFloatMemDivide),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //------------Double Div in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_DIVSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_DIVSD_XMMsd_XMMsd)
       )
       //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(PreRed_EmulateDoubleRegDivide),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Double Div in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_DIVSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_DIVSD_XMMsd_MEMsd) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(PreRed_EmulateDoubleMemDivide),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //-----------------------------------------//
    //------------END DIV TESTS--------------//
    //-----------------------------------------//
    //-----------------------------------------//
    //------------START MUL TESTS--------------//
    //-----------------------------------------//
    //------------Float Mul in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_MULSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MULSS_XMMss_XMMss)
       )
        //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(PreRed_EmulateFloatRegMultiply),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Float Mul in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_MULSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MULSS_XMMss_MEMss) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(PreRed_EmulateFloatMemMultiply),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //------------Double Mul in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_MULSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MULSD_XMMsd_XMMsd)
       )
       //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(PreRed_EmulateDoubleRegMultiply),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Double Mul in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_MULSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MULSD_XMMsd_MEMsd) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(PreRed_EmulateDoubleMemMultiply),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //-----------------------------------------//
    //------------END MUL TESTS--------------//
    //-----------------------------------------//
    //-----------------------------------------//
    //------------START ADD TESTS--------------//
    //-----------------------------------------//
    //------------Float Add in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_ADDSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_ADDSS_XMMss_XMMss)
       )
        //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(PreRed_EmulateFloatRegSum),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Float Add in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_ADDSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_ADDSS_XMMss_MEMss) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(PreRed_EmulateFloatMemSum),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //------------Double Add in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_ADDSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_ADDSD_XMMsd_XMMsd)
       )
       //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(PreRed_EmulateDoubleRegSum),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Double Add in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_ADDSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_ADDSD_XMMsd_MEMsd) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(PreRed_EmulateDoubleMemSum),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //-----------------------------------------//
    //------------END ADD TESTS--------------//
    //-----------------------------------------//
    //-----------------------------------------//
    //------------START SUB TESTS--------------//
    //-----------------------------------------//
    //------------Float Sub in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_SUBSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_SUBSS_XMMss_XMMss)
       )
        //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(PreRed_EmulateFloatRegDifference),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Float Sub in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_SUBSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_SUBSS_XMMss_MEMss) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(PreRed_EmulateFloatMemDifference),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }
    //------------Double Sub in REGS------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_SUBSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_SUBSD_XMMsd_XMMsd)
       )
       //(INS_OperandIsReg(ins, 0)))
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                  IPOINT_BEFORE,
                  AFUNPTR(PreRed_EmulateDoubleRegDifference),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                  IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                  IARG_CONTEXT,
                  IARG_THREAD_ID,
                  IARG_END);
        INS_Delete(ins);
    }
    //------------Double Sub in MEM------------//
    else if ((INS_Opcode(ins) == XED_ICLASS_SUBSD) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_SUBSD_XMMsd_MEMsd) &&
        (INS_IsMemoryRead(ins))
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                    IPOINT_BEFORE,
                    AFUNPTR(PreRed_EmulateDoubleMemDifference),
                    IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_CONTEXT,
                    IARG_THREAD_ID,
                    IARG_END);
        INS_Delete(ins);
    }

    /*
    if( (INS_Mnemonic(ins) == "FDIV")         // &&
        //(
        //  (INS_Opcode(ins) != XED_ICLASS_IDIV)  ||
        //  (INS_Opcode(ins) != XED_ICLASS_DIVSS) ||
        //  (INS_Opcode(ins) != XED_ICLASS_DIVSD)
        //)
      )
    {
        //InstructionDecodeFunc(ins);
    }
    */
    
    //-----------------------------------------//
    //------------END SUB TESTS--------------//
    //-----------------------------------------//
    //------------START AxC LD/ST TESTS--------------//
    //-----------------------------------------//
    
    //Load floats
    //Floating point (float) move from memory to register
    if ((INS_Opcode(ins) == XED_ICLASS_MOVSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MOVSS_XMMdq_MEMss)
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       AFUNPTR(PreRed_DoLoad_MOVSS_XMM_MEM),
                       IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                       IARG_MEMORYREAD_EA,
                       IARG_UINT32, REG(INS_OperandReg(ins, 0)),
                       //IARG_RETURN_REGS, INS_OperandReg(ins, 0),
                       IARG_END);
        INS_Delete(ins);
    }

    //Store floats
    //Floating point (float) move from register to memory
    if ((INS_Opcode(ins) == XED_ICLASS_MOVSS) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MOVSS_MEMss_XMMss)
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       AFUNPTR(PreRed_DoLoad_MOVSS_MEM_XMM),
                       IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                       IARG_MEMORYWRITE_EA,
                       IARG_UINT32, REG(INS_OperandReg(ins,1)),
                       //IARG_RETURN_REGS, INS_OperandReg(ins, 0),
                       IARG_END);
        INS_Delete(ins);
    }
    
    //Load doubles
    //Floating point (double) move from memory to register
    if ((INS_Opcode(ins) == XED_ICLASS_MOVSD_XMM) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MOVSD_XMM_XMMdq_MEMsd)
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       AFUNPTR(PreRed_DoLoad_MOVSD_XMM_MEM),
                       IARG_REG_REFERENCE, REG(INS_OperandReg(ins,0)),
                       IARG_MEMORYREAD_EA,
                       IARG_UINT32, REG(INS_OperandReg(ins, 0)),
                       //IARG_RETURN_REGS, INS_OperandReg(ins, 0),
                       IARG_END);
        INS_Delete(ins);
    }

    //Store doubles
    //Floating point (double) move from register to memory
    if ((INS_Opcode(ins) == XED_ICLASS_MOVSD_XMM) &&
        (xed_decoded_inst_get_iform_enum(INS_XedDec(ins)) == XED_IFORM_MOVSD_XMM_MEMsd_XMMsd)
       )
    {
        //InstructionDecodeFunc(ins);
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       AFUNPTR(PreRed_DoLoad_MOVSD_MEM_XMM),
                       IARG_REG_REFERENCE, REG(INS_OperandReg(ins,1)),
                       IARG_MEMORYWRITE_EA,
                       IARG_UINT32, REG(INS_OperandReg(ins,1)),
                       //IARG_RETURN_REGS, INS_OperandReg(ins, 0),
                       IARG_END);
        INS_Delete(ins);
    }

}

/* ===================================================================== */
/* Instrument the cache heirarchy                                        */   
/* ===================================================================== */

/* ===================================================================== */

VOID simulate_cacheHierarchy(INS ins, UINT64* functionL1AccessCount, UINT64* functionL1MissCount, UINT64* functionL2AccessCount, UINT64* functionL2MissCount)
{
    
    UINT32 memOperands = INS_MemoryOperandCount(ins);
    
    // Instrument each memory operand. If the operand is both read and written
    // it will be processed twice.
    // Iterating over memory operands ensures that instructions on IA-32 with
    // two read operands (such as SCAS and CMPS) are correctly handled.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        const UINT32 size = INS_MemoryOperandSize(ins, memOp);
        const BOOL   single = (size <= 4);
        
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            if( single )
            {
                INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE,  (AFUNPTR) LoadSingleFast_AxC,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_PTR, functionL1AccessCount,
                    IARG_PTR, functionL1MissCount,
                    IARG_PTR, functionL2AccessCount,
                    IARG_PTR, functionL2MissCount,
                    IARG_END);
                    
            }
            else
            {
                INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE,  (AFUNPTR) LoadMultiFast_AxC,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_UINT32, size,
                    IARG_PTR, functionL1AccessCount,
                    IARG_PTR, functionL1MissCount,
                    IARG_PTR, functionL2AccessCount,
                    IARG_PTR, functionL2MissCount,
                    IARG_END);
            }
        }
        
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            if( single )
            {
                INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE,  (AFUNPTR) StoreSingleFast_AxC,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_PTR, functionL1AccessCount,
                    IARG_PTR, functionL1MissCount,
                    IARG_PTR, functionL2AccessCount,
                    IARG_PTR, functionL2MissCount,
                    IARG_END);
                    
            }
            else
            {
                INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE,  (AFUNPTR) StoreMultiFast_AxC,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_UINT32, size,
                    IARG_PTR, functionL1AccessCount,
                    IARG_PTR, functionL1MissCount,
                    IARG_PTR, functionL2AccessCount,
                    IARG_PTR, functionL2MissCount,
                    IARG_END);
            }
        }
    }  //for

}


//AxC cache simulation
VOID AxC_cacheHierarchy(INS ins, UINT64* functionL1AccessCount, UINT64* functionL1MissCount, UINT64* functionL2AccessCount, UINT64* functionL2MissCount)
{
    //For now AxC and PxC use the same cache
    simulate_cacheHierarchy(ins, functionL1AccessCount, functionL1MissCount, functionL2AccessCount, functionL2MissCount);

}

//PxC cache simulation
VOID PxC_cacheHierarchy(INS ins, UINT64* functionL1AccessCount, UINT64* functionL1MissCount, UINT64* functionL2AccessCount, UINT64* functionL2MissCount)
{

    //For now AxC and PxC use the same cache
    simulate_cacheHierarchy(ins, functionL1AccessCount, functionL1MissCount, functionL2AccessCount, functionL2MissCount);

}

/* ===================================================================== */
/* Instrument AxC Function instructions                                  */   
/* ===================================================================== */

/* ===================================================================== */

VOID instrumentInstructionAxC(INS ins, UINT64* functionL1AccessCount, UINT64* functionL1MissCount, UINT64* functionL2AccessCount, UINT64* functionL2MissCount)
{

    //Instrument instructions
    Noise_InstrumentArithmeticFunction(ins);
    //InstrumentDivideFunction(ins);
    
    //Instrument the cache heirarchy
    AxC_cacheHierarchy(ins, functionL1AccessCount, functionL1MissCount, functionL2AccessCount, functionL2MissCount);

}    

/* ===================================================================== */
/* Instrument PxC Function instructions                                  */   
/* ===================================================================== */

/* ===================================================================== */

VOID instrumentInstructionPxC(INS ins, UINT64* functionL1AccessCount, UINT64* functionL1MissCount, UINT64* functionL2AccessCount, UINT64* functionL2MissCount)
{

    //Instrument instructions
    PrecisionReduction_InstrumentArithmeticFunction(ins);
    //InstrumentDivideFunction(ins);
    
    //Instrument the cache heirarchy
    PxC_cacheHierarchy(ins, functionL1AccessCount, functionL1MissCount, functionL2AccessCount, functionL2MissCount);

}

/* ===================================================================== */
/* Instrument Non- AxC Function instructions                             */   
/* ===================================================================== */

/* ===================================================================== */

VOID instrumentInstructionMainCache(INS ins, UINT64* functionL1AccessCount, UINT64* functionL1MissCount, UINT64* functionL2AccessCount, UINT64* functionL2MissCount)
{

    //No code instrumentation for non-axc and non-pxc functions, hence just simulate the cache
    UINT32 memOperands = INS_MemoryOperandCount(ins);
    
    // Instrument each memory operand. If the operand is both read and written
    // it will be processed twice.
    // Iterating over memory operands ensures that instructions on IA-32 with
    // two read operands (such as SCAS and CMPS) are correctly handled.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        const UINT32 size = INS_MemoryOperandSize(ins, memOp);
        const BOOL   single = (size <= 4);
        
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            if( single )
            {
                INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE,  (AFUNPTR) LoadSingleFast,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_PTR, functionL1AccessCount,
                    IARG_PTR, functionL1MissCount,
                    IARG_PTR, functionL2AccessCount,
                    IARG_PTR, functionL2MissCount,
                    IARG_END);
                    
            }
            else
            {
                INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE,  (AFUNPTR) LoadMultiFast,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_UINT32, size,
                    IARG_PTR, functionL1AccessCount,
                    IARG_PTR, functionL1MissCount,
                    IARG_PTR, functionL2AccessCount,
                    IARG_PTR, functionL2MissCount,
                    IARG_END);
            }
        }
        
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            if( single )
            {
                INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE,  (AFUNPTR) StoreSingleFast,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_PTR, functionL1AccessCount,
                    IARG_PTR, functionL1MissCount,
                    IARG_PTR, functionL2AccessCount,
                    IARG_PTR, functionL2MissCount,
                    IARG_END);
                    
            }
            else
            {
                INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE,  (AFUNPTR) StoreMultiFast,
                    IARG_MEMORYOP_EA, memOp,
                    IARG_UINT32, size,
                    IARG_PTR, functionL1AccessCount,
                    IARG_PTR, functionL1MissCount,
                    IARG_PTR, functionL2AccessCount,
                    IARG_PTR, functionL2MissCount,
                    IARG_END);
            }
        }
    } //for

}

/* ===================================================================== */
/* Test whether a routine is an AxC specified routine                    */   
/* ===================================================================== */

/* ===================================================================== */

bool isAxCFunction(string functionName)
{
    
    if(KnobVanillaSimulationMode)
    {
        return(false);
    }
    else if( ( mAxCFunctionList.find(functionName) != mAxCFunctionList.end() ) )
    {   
        //Full string match - finds "functionName" in the "List"
	    cout << "\nFound AxC:  " << functionName << "\n" << endl;
	    return(true);
    }
    //else
	//    return(false);
    else
    {   
        //Partial string match - finds "List" in the "functionName" -- opposite of what the previous thing did
        std::set<string>::const_iterator itr;
        string string_in_functionList;
        size_t pos;
        
        for(itr = mAxCFunctionList.begin(); itr != mAxCFunctionList.end(); itr++)
        {
            string_in_functionList = *itr;
            //finds a match in "functionName" from among the names stored in "mAxCFunctionList"
            pos = functionName.find(string_in_functionList); 
            if(pos != string::npos)
            {
                cout << "\nFound AxC2: " << functionName << "\n" << endl;
	            return(true); 
            }
        }
    
        return(false);
    }

}

/* ===================================================================== */
/* Test whether a routine is a PxC specified routine                    */   
/* ===================================================================== */

/* ===================================================================== */

bool isPxCFunction(string functionName)
{
    
    if(KnobVanillaSimulationMode)
    {
        return(false);
    }
    else if( ( mPxCFunctionList.find(functionName) != mPxCFunctionList.end() ) )
    {   
        //Full string match - finds "functionName" in the "List"
	    cout << "\nFound PxC:  " << functionName << "\n" << endl;
	    return(true);
    }
    //else
	//    return(false);
    else
    {   
        //Partial string match - finds "List" in the "functionName" -- opposite of what the previous thing did
        std::set<string>::const_iterator itr;
        string string_in_functionList;
        size_t pos;
        
        for(itr = mPxCFunctionList.begin(); itr != mPxCFunctionList.end(); itr++)
        {
            string_in_functionList = *itr;
            //finds a match in "functionName" from among the names stored in "mPxCFunctionList"
            pos = functionName.find(string_in_functionList); 
            if(pos != string::npos)
            {
                cout << "\nFound PxC2: " << functionName << "\n" << endl;
	            return(true); 
            }
        }
    
        return(false);
    }

}

/* ===================================================================== */
/* Choose where to instrument instructions                               */   
/* ===================================================================== */

/* ===================================================================== */

VOID instrumentInstruction(INS ins, UINT64* functionL1AccessCount, UINT64* functionL1MissCount, UINT64* functionL2AccessCount, UINT64* functionL2MissCount, bool isAxC, bool isPxC)
{

    if(isAxC)
    {
        //cout << "AxC ";
        //InstructionDecodeFunc(ins);
        instrumentInstructionAxC(ins, functionL1AccessCount, functionL1MissCount, functionL2AccessCount, functionL2MissCount);
        //return;
    }
    
    if(isPxC)
    {
        //cout << "PxC ";
        //InstructionDecodeFunc(ins);
        instrumentInstructionPxC(ins, functionL1AccessCount, functionL1MissCount, functionL2AccessCount, functionL2MissCount);
        //return;
    }
    
    //else
    if( !isAxC && !isPxC )
    {
        //cout << "Main ";
        //InstructionDecodeFunc(ins);
        //No code instrumentation for non-axc and non-pxc functions, hence just simulate the cache
        instrumentInstructionMainCache(ins, functionL1AccessCount, functionL1MissCount, functionL2AccessCount, functionL2MissCount);
        //return;
    }

}        

/* ===================================================================== */

// This function is called before every routine is executed
VOID docount(UINT64 * counter)
{
    
    (*counter)++;

}
    
const char * StripPath(const char * path)
{

    const char * file = strrchr(path,'/');
    if (file)
        return file+1;
    else
        return path;

}

/* ===================================================================== */

// Pin calls this function every time a new rtn is executed
VOID FunctionRoutineAnalysis(RTN rtn, VOID *v)
{
    
    // Allocate a counter for this routine
    RTN_COUNT * rc = new RTN_COUNT;

    // The RTN goes away when the image is unloaded, so save it now
    // because we need it in the fini
    rc->_name             = RTN_Name(rtn);
    rc->_image            = StripPath(IMG_Name(SEC_Img(RTN_Sec(rtn))).c_str());
    rc->_address          = RTN_Address(rtn);
    rc->_rtnCount         = 0;
    rc->_rtnL1AccessCount = 0;
    rc->_rtnL1MissCount   = 0;
    rc->_rtnL2AccessCount = 0;
    rc->_rtnL2MissCount   = 0;
    rc->_icount           = 0;
    
    // Figure out AxC functions etc.
    string deMangledName = PIN_UndecorateSymbolName(rc->_name, UNDECORATION_NAME_ONLY); // PIN demangles the function names here
    
    //bool isAxC  = isAxCFunction(rc->_name) | isAxCFunction(deMangledName);
    bool isAxC    = isAxCFunction(deMangledName) ? 1 : ( isAxCFunction(rc->_name) ? 1 : 0 );
    rc->_RTNisAxC = isAxC;
    
    bool isPxC    = isPxCFunction(deMangledName) ? 1 : ( isPxCFunction(rc->_name) ? 1 : 0 );
    rc->_RTNisPxC = isPxC;
  
    // Add to list of routines
    rc->_next = RtnList;
    RtnList   = rc;
    
    if( RTN_Valid(rtn))
    {
        RTN_Open(rtn);
            
        // Insert a call at the entry point of a routine to increment the call count
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)docount, IARG_PTR, &(rc->_rtnCount), IARG_END);
        
        // For each instruction of the routine
        for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
        {
            // Insert a call to docount to increment the instruction counter for this rtn
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_PTR, &(rc->_icount), IARG_END);
            // Call a PIN function that instruments the memory heirarchy of the application function or routine
            instrumentInstruction(ins, &(rc->_rtnL1AccessCount), &(rc->_rtnL1MissCount), &(rc->_rtnL2AccessCount), &(rc->_rtnL2MissCount), isAxC, isPxC);
        }
        
        RTN_Close(rtn);
    }

}


/* ===================================================================== */

// This function is called when the application exits
// It prints some statistics for each procedure (function)
VOID FiniFunctionProfiling(int code, VOID *v)
{

    outFile << "FUNCTION_PROFILE_BEGIN\n";

    outFile << ">Procedure" << "\t"
            << "Image" << "\t"
            << "isAxC" << "\t"
            << "isPxC" << "\t"
            << "Count" << "\t"
            << "Instr" << "\t"
            << "L1AccessCount" << "\t"
            << "L1MissCount" << "\t"
            << "L2AccessCount" << "\t"
            << "L2MissCount" << endl;

    for (RTN_COUNT * rc = RtnList; rc; rc = rc->_next)
    {
        if (rc->_icount > 0)
            outFile << PIN_UndecorateSymbolName(rc->_name, UNDECORATION_NAME_ONLY) << "\t"
                    << rc->_image << "\t"
                    << rc->_RTNisAxC << "\t"
                    << rc->_RTNisPxC << "\t"
                    << rc->_rtnCount << dec <<"\t"
                    << rc->_icount << dec <<"\t"
                    << rc->_rtnL1AccessCount << dec << "\t"
                    << rc->_rtnL1MissCount << dec << "\t"
                    << rc->_rtnL2AccessCount << dec << "\t"
                    << rc->_rtnL2MissCount << endl; 
        
    }
    
    outFile<< "FUNCTION_PROFILE_END\n";

}

/* ===================================================================== */

// This function is called when the application exits
// It prints the cache hierarchy stats of the simulated architecture
VOID FiniCacheProfiling(int code, VOID *v)
{

    outFile << "# DL1 CACHE configuration ["
            << "csize = " << dl1->CacheSize() / 1024 << "KB, "
            << "lsize = " << dl1->LineSize() << "B, "
            << "assoc = " << dl1->Associativity() << "]\n";
    outFile << "# AxC CACHE configuration ["
            << "csize = " << d_axc->CacheSize() / 1024 << "KB, "
            << "lsize = " << d_axc->LineSize() << "B, "
            << "assoc = " << d_axc->Associativity() << "]\n";
    outFile << "# UL2 CACHE configuration ["
            << "csize = " << ul2->CacheSize() / 1024 << "KB, "
            << "lsize = " << ul2->LineSize() << "B, "
            << "assoc = " << ul2->Associativity() << "]\n";
    outFile << endl; 
    
    outFile << dl1->StatsLong("#DL1 ", CACHE_BASE::CACHE_TYPE_DCACHE);
    outFile << d_axc->StatsLong("#AxC ", CACHE_BASE::CACHE_TYPE_DCACHE);
    outFile << ul2->StatsLong("#UL2 ", CACHE_BASE::CACHE_TYPE_DCACHE);

}

/* ===================================================================== */

//Function called when application exists
VOID Fini(int code, VOID *v)
{

    //std::ofstream out(KnobOutputFile.Value().c_str());
    outFile.open(KnobOutputFile.Value().c_str());

    // Cache stats 
    FiniCacheProfiling(code, v);

    // Funtion stats
    FiniFunctionProfiling(code, v);
    
    outFile << "\nProbability of bit flips for noisy computation (if done) = " << ProbBitFlips << "\n" << endl;

    outFile.close();

}

/* ===================================================================== */

// This function reads the user specified list of AxC functions from a file 

void readAxCFunctionList(set<string> &axcFunctionList)
{                                                                          
    
    std::ifstream myReadFile;                   
    myReadFile.open(KnobInputAxCFile.Value().c_str());           
    
    if(myReadFile)
    {
        cout << "\n\n";
        string output;
        
        while (myReadFile >> output)
        {
            //cout << "Read line                  : " << output << endl;
        	if(output.find("#") != string::npos)
            {
                cout << "Ignoring comment line:       " << output << "\n";
            }
            else //if "#" is not found in the string
            {
                cout << "Adding AxC Function to Arch: " << output << "\n";
        	    axcFunctionList.insert(output); 
            }
        }
        cout << "\n\n";

        //Print this data here since this probability makes sense only if AxC knob is specified
        cout << "Probability of bit flips for noisy computation = " << ProbBitFlips << "\n" << endl;
    }
    else
    {
        //Couldnt open the file
        cout << "No AxC file specified or file could not be opened\n" << endl;
    }
    
    myReadFile.close();

}                 

// This function reads the user specified list of PxC functions from a file 

void readPxCFunctionList(set<string> &pxcFunctionList)
{                                                                          
    
    std::ifstream myReadFile;                   
    myReadFile.open(KnobInputPxCFile.Value().c_str());           
    
    if(myReadFile)
    {
        cout << "\n\n";
        string output;
        
        while (myReadFile >> output)
        {
            //cout << "Read line                  : " << output << endl;
        	if(output.find("#") != string::npos)
            {
                cout << "Ignoring comment line:       " << output << "\n";
            }
            else //if "#" is not found in the string
            {
                cout << "Adding PxC Function to Arch: " << output << "\n";
        	    pxcFunctionList.insert(output); 
            }
        }
        cout << "\n\n";
    }
    else
    {
        //Couldnt open the file
        cout << "No PxC file specified or file could not be opened\n" << endl;
    }
    
    myReadFile.close();

}                 


/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    
    // Initialize symbol table code, needed for rtn instrumentation
    PIN_InitSymbols();

    // Initialize pin
    if( PIN_Init(argc,argv) )
        return Usage();

    //Initialize cache structures
    dl1 = new DL1::CACHE("L1 Data Cache", 
                         KnobDL1CacheSize.Value() * KILO,
                         KnobDL1LineSize.Value(),
                         KnobDL1Associativity.Value());
    
    d_axc = new D_AxC::CACHE("AxC L1 Data Cache", 
                         KnobAxCCacheSize.Value() * KILO,
                         KnobAxCLineSize.Value(),
                         KnobAxCAssociativity.Value());
    
    ul2 = new UL2::CACHE("UL2 Data Cache", 
                         KnobUL2CacheSize.Value() * KILO,
                         KnobUL2LineSize.Value(),
                         KnobUL2Associativity.Value());
 
    //Take in the user specified probability of bit flips
    ProbBitFlips = KnobProbabilityBitFlips.Value();

    if( (ProbBitFlips < 0.0) || (ProbBitFlips > 1.0) )
    {
        cout << "Probability of bit flips should be between 0.0 and 1.0\n" << endl;    
        return Usage();
    }

    //Read the list of AxC functions from axcFunctionList.txt file
    readAxCFunctionList(mAxCFunctionList);
    
    //Read the list of PxC functions from pxcFunctionList.txt file
    readPxCFunctionList(mPxCFunctionList);

    //Register Routine to be called to instrument rtn
    //INS_AddInstrumentFunction(Instruction, 0);
    RTN_AddInstrumentFunction(FunctionRoutineAnalysis, 0);
    
    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;

}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
