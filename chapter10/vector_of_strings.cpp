/*
 * Copyright (c) 2020 Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * vector_of_strings.cpp - demonstrate a vector of strings in pmem
 */

#include <pmem_allocator.h>
#include <vector>
#include <string>
#include <scoped_allocator>
#include <cassert>
#include <iostream>

typedef libmemkind::pmem::allocator<char> str_alloc_type;

typedef std::basic_string<char, std::char_traits<char>, str_alloc_type> pmem_string;

typedef libmemkind::pmem::allocator<pmem_string> vec_alloc_type;

typedef std::vector<pmem_string, std::scoped_allocator_adaptor<vec_alloc_type> > vector_type;

int main(int argc, char *argv[]) {
	const size_t pmem_max_size = 64 * 1024 * 1024; //64 MB
	const std::string pmem_dir("/daxfs");

	// Create allocator object
	vec_alloc_type alc(pmem_dir, pmem_max_size);
	// Create std::vector with our allocator.
	vector_type v(alc);

	v.emplace_back("Foo");
	v.emplace_back("Bar");

	for (auto str : v) {
		std::cout << str << std::endl;
	}

	exit(0);
}
