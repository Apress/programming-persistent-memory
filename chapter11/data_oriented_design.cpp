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

#include "libpmemobj++/array.hpp"
#include "libpmemobj++/pool.hpp"
#include "libpmemobj++/transaction.hpp"

struct soa {
	int a[1000];
	int b[1000];
};

struct root {
	soa soa_records;
	std::pair<int, int> aos_records[1000];
};

int main()
{
	pmem::obj::pool<root> pop;

	try {
		pop = pmem::obj::pool<root>::create("pmpool", "data_oriented",
						PMEMOBJ_MIN_POOL, 0666);

		auto root = pop.root();

		pmem::obj::transaction::run(pop, [&]{
			pmem::obj::transaction::snapshot(&root->soa_records);
			for (int i = 0; i < 1000; i++) {
				root->soa_records.a[i]++;
			}

			for (int i = 0; i < 1000; i++) {
				pmem::obj::transaction::snapshot(
					&root->aos_records[i].first);
				root->aos_records[i].first++;
			}
		});
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	pop.close();

	return 0;
}

