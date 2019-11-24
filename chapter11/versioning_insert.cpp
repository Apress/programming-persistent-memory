/*
 * Copyright 2019, Intel Corporation
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
 *     * Neither the name of the copyright holder nor the names of its
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* This is simplified version of insert operation from b+tree:
 * https://github.com/pmem/pmemkv/blob/master/src/engines-experimental/stree/persistent_b_tree.h */

#include <iostream>

#include "libpmemobj++/pool.hpp"
#include "libpmemobj++/persistent_ptr.hpp"

template <typename Value, uint64_t slots>
struct entries_t {
        Value entries[slots];
        size_t size;
};

template <typename Value, uint64_t slots>
class array {
public:
	void insert(pmem::obj::pool_base &pop, const Value &entry);
	void insert_element(pmem::obj::pool_base &pop, const Value &entry);

        entries_t<Value, slots> v[2];
        uint32_t current;
};

template <typename Value, uint64_t slots>
void array<Value, slots>::insert_element(pmem::obj::pool_base &pop,
					const Value &entry) {
	auto &working_copy = v[1 - current];
	auto &consistent_copy = v[current];

	auto consistent_insert_position = std::lower_bound(
		std::begin(consistent_copy.entries),
		std::begin(consistent_copy.entries) + consistent_copy.size,
		entry);
	auto working_insert_position = std::begin(working_copy.entries) +
		std::distance(std::begin(consistent_copy.entries),
		consistent_insert_position);

	std::copy(std::begin(consistent_copy.entries),
		  consistent_insert_position,
		  std::begin(working_copy.entries));

	*working_insert_position = entry;

	std::copy(consistent_insert_position,
		  std::begin(consistent_copy.entries) + consistent_copy.size,
		  working_insert_position + 1);

	working_copy.size = consistent_copy.size + 1;
}

template <typename Value, uint64_t slots>
void array<Value, slots>::insert(pmem::obj::pool_base &pop,
			 const Value &entry){
	insert_element(pop, entry);
	pop.persist(&(v[1 - current]), sizeof(entries_t<Value, slots>));

	current = 1 - current;
	pop.persist(&current, sizeof(current));
}

int main()
{
	pmem::obj::pool<array<int, 10>> pop;

	try {
		pop = pmem::obj::pool<array<int, 10>>::create("/daxfs/pmpool", "versioning_insert",
						PMEMOBJ_MIN_POOL, 0666);

		auto root = pop.root();

		root->insert(pop, 0);
		root->insert(pop, 5);
		root->insert(pop, 3);
		root->insert(pop, 1);
		root->insert(pop, 2);

		for (int i = 0; i < root->v[root->current].size; i++)
			std::cout << root->v[root->current].entries[i];

	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	pop.close();

	return 0;
}
