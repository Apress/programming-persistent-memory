# Copyright (c) 2020, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#
#     * Neither the name of Intel Corporation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#
# Makefile for chapter11 examples
#

.SUFFIXES: .lst

PROGS = data_oriented_design simplekv simplekv_rebuild versioning_insert

all: $(PROGS) listings

listings: simplekv.lst simplekv_rebuild.lst data_oriented_design.lst versioning_insert.lst

simplekv.lst: simplekv.hpp
	cat -n $^ > $@

simplekv_rebuild.lst: simplekv_rebuild.hpp
	cat -n $^ > $@

data_oriented_design.lst: data_oriented_design.cpp
	cat -n $^ > $@

versioning_insert.lst: versioning_insert.cpp
	cat -n $^ > $@

simplekv: simplekv.cpp
	$(CXX) -o simplekv simplekv.cpp -lpmemobj

simplekv_rebuild: simplekv_rebuild.cpp
	$(CXX) -o simplekv_rebuild simplekv_rebuild.cpp -lpmemobj

data_oriented_design: data_oriented_design.cpp
	$(CXX) -o data_oriented_design data_oriented_design.cpp -lpmemobj

versioning_insert: versioning_insert.cpp
	$(CXX) -o versioning_insert versioning_insert.cpp -g -lpmemobj

clean:
	$(RM) *.o core a.out

clobber: clean
	$(RM) $(PROGS) *.lst

.PHONY: all clean clobber listings
