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

CC = gcc
CXX = g++
RM = rm -f

TARGETS=leak stackoverflow listing_12-9 listing_12-11 listing_12-13 listing_12-16 listing_12-17 listing_12-17 listing_12-23 listing_12-25 listing_12-28 listing_12-33 listing_12-36 listing_12-38 listing_12-40 listing_12-44 listing_12-45 listing_12-48 listing_12-51
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
