#//==============================================================================
#//Copyright (c) 2013 Intel Corporation.
#//All rights reserved.
#//==============================================================================
#//
#//Developed by:
#//
#//    Accelerator Architecture Lab, Intel Labs
#//
#//    Intel Corporation
#//
#//    http://www.intel.com
#//
#//Permission is hereby granted, free of charge, to any person obtaining a copy of
#//this software and associated documentation files (the "Software"), to deal with
#//the Software without restriction, including without limitation the rights to
#//use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
#//of the Software, and to permit persons to whom the Software is furnished to do
#//so, subject to the following conditions:
#//
#//    * Redistributions of source code must retain the above copyright notice,
#//      this list of conditions and the following disclaimers.
#//
#//    * Redistributions in binary form must reproduce the above copyright notice,
#//      this list of conditions and the following disclaimers in the
#//      documentation and/or other materials provided with the distribution.
#//
#//    * Neither the names of the LLVM Team, University of Illinois at
#//      Urbana-Champaign, nor the names of its contributors may be used to
#//      endorse or promote products derived from this Software without specific
#//      prior written permission.
#//
#//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
#//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
#//CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
#//SOFTWARE.
#//==============================================================================
#//Contact: Asit K Mishra (asit.k.mishra@intel.com)


# This top-level makefile is exported into the Pin kit and is referenced by
# several external scripts.  Be careful when changing the names of the makefile
# targets so as not to break anything that depends on them.

# All directories which contain tests should be placed here.
# Please maintain alphabetical order.

ALL_TEST_DIRS := AxC-PINTOOL 

# Some of these targets are referenced in the User Guide and/or are commonly used by Pin users.
# Others are used by the nightly system and/or remote-builder.py.

all: build

utils:
	$(MAKE) -C Utils

build: utils
	-$(MAKE) -k $(ALL_TEST_DIRS:%=%.build)

install: Utils.install
	-$(MAKE) -k $(ALL_TEST_DIRS:%=%.install)

test: $(ALL_TEST_DIRS:%=%.test)
	Utils/testsummary

sanity: $(ALL_TEST_DIRS:%=%.sanity)
	Utils/testsummary

clean: $(ALL_TEST_DIRS:%=%.clean)
	-$(MAKE) -k -C Utils clean

# These are directory-specific template targets.

$(ALL_TEST_DIRS:%=%.build):
	-$(MAKE) -k -C $(@:%.build=%)

$(ALL_TEST_DIRS:%=%.install):
	-$(MAKE) -k -C $(@:%.install=%) install

Utils.install:
	$(MAKE) -k -C $(@:%.install=%) install

$(ALL_TEST_DIRS:%=%.test):
	-$(MAKE) -k -C $(@:%.test=%) test

$(ALL_TEST_DIRS:%=%.sanity):
	-$(MAKE) -k -C $(@:%.sanity=%) sanity

$(ALL_TEST_DIRS:%=%.clean):
	-$(MAKE) -k -C $(@:%.clean=%) clean

.PHONY: all utils build install test sanity clean
.PHONY: $(ALL_TEST_DIRS:%=%.build) $(ALL_TEST_DIRS:%=%.install) $(ALL_TEST_DIRS:%=%.test)
.PHONY: $(ALL_TEST_DIRS:%=%.sanity) $(ALL_TEST_DIRS:%=%.clean)
