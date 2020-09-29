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
# Makefile for chapter8 examples
#

.SUFFIXES: .lst

all: transaction p allocation listings non_trivial_copy volatile_pointers queue containers

listings: transaction.lst p.lst allocation.lst non_trivial_copy.lst volatile_pointers.lst persistent_queue.lst volatile_queue.lst queue.lst containers.lst

%.lst: %.cpp
	cat -n $^ > $@

%.lst: %.hpp
	cat -n $^ > $@

transaction: transaction.cpp
	$(CXX) -std=c++11 -o transaction transaction.cpp -lpmemobj

p: p.cpp
	$(CXX) -std=c++11 -o p p.cpp -lpmemobj

allocation: allocation.cpp
	$(CXX) -std=c++11 -o allocation allocation.cpp -lpmemobj

non_trivial_copy: non_trivial_copy.cpp
	$(CXX) -std=c++11 -c -o non_trivial_copy non_trivial_copy.cpp

volatile_pointers: volatile_pointers.cpp
	$(CXX) -std=c++11 -c -o volatile_pointers volatile_pointers.cpp

queue: queue.cpp
	$(CXX) -std=c++11 -o queue queue.cpp -lpmemobj

containers: containers.cpp
	$(CXX) -std=c++11 -o containers containers.cpp -lpmemobj

clean:
	$(RM) *.o core a.out

clobber: clean
	$(RM) transaction p allocation volatile_pointers queue non_trivial_copy containers *.lst

.PHONY: all clean clobber listings
