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

template <uint64_t slots>
struct leaf_entries_t {
        uint64_t idx[slots];
        size_t size;
};

template <typename Key, typename Value, uint64_t slots>
class leaf_node_t {
public:
        using key_type = Key;
        using value_type = std::pair<key_type, value_type>;
private:
        value_type entries[slots];
        leaf_entries_t v[2];
        uint32_t current;
};

void leaf_node_t::insert(pool_base &pop,
			 const value_type &entry){
	auto insert_pos = v[current].size;

	entries[insert_pos] = entry;
	pop.flush(&(entries[insert_pos]),
			  sizeof(entries[insert_pos]));

	insert_idx(pop, insert_pos);
	pop.persist(&(v[1 - current]), sizeof(leaf_entries_t));

	current = 1 - current;
	pop.persist(&current, sizeof(current));
}
