# Copyright (c) 2019, Intel Corporation
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

CC = gcc
CXX = g++
RM = rm -f

TARGETS=leak stackoverflow listing_8-9 listing_8-11 listing_8-13 listing_8-16 listing_8-17 listing_8-17 listing_8-23 listing_8-25 listing_8-28 listing_8-33 listing_8-36 listing_8-38 listing_8-40 listing_8-44 listing_8-45 listing_8-48 listing_8-51
TARGETS_LISTINGS = $(addsuffix .lst, $(TARGETS))

all: $(TARGETS) listings
listings: $(TARGETS_LISTINGS)

%: %.c
	$(CC) -g -o $@ $< -lpthread -lpmemobj

%: %.cpp
	$(CXX) -g -o $@ $< -lpthread -lpmemobj -std=c++11

%.lst: %.c
	cat -n $^ > $@

%.lst: %.cpp
	cat -n $^ > $@

clean:
	$(RM) $(TARGETS) $(TARGETS_LISTINGS)
