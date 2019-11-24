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

#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/container/vector.hpp>

using vector_type = pmem::obj::vector<int>;

struct root {
        pmem::obj::persistent_ptr<vector_type> vec_p;
};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " path_to_pool" << std::endl;
		return 1;
	}

	auto path = argv[1];
	pmem::obj::pool<root> pool;

	try {
		pool = pmem::obj::pool<root>::open(path, "vector");
	} catch(pmem::pool_error &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "To create pool run: pmempool create obj --layout=vector -s 100M path_to_pool" << std::endl;
	}

	auto root = pool.root();

	/* creating pmem::obj::vector in transaction */
	pmem::obj::transaction::run(pool, [&] {
		root->vec_p = pmem::obj::make_persistent<vector_type>(/* optional constructor arguments */);
	});

	vector_type &pvector = *(root->vec_p);

	pvector.reserve(10);
	assert(pvector.size() == 0);
	assert(pvector.capacity() == 10);

	pvector = {0, 1, 2, 3, 4};
	assert(pvector.size() == 5);
	assert(pvector.capacity() == 10);

	pvector.shrink_to_fit();
	assert(pvector.size() == 5);
	assert(pvector.capacity() == 5);

	for (unsigned i = 0; i < pvector.size(); ++i)
		assert(pvector.const_at(i) == static_cast<int>(i));

	pvector.push_back(5);
	assert(pvector.const_at(5) == 5);
	assert(pvector.size() == 6);

	pvector.emplace(pvector.cbegin(), pvector.back());
	assert(pvector.const_at(0) == 5);
	for (unsigned i = 1; i < pvector.size(); ++i)
		assert(pvector.const_at(i) == static_cast<int>(i - 1));

	std::vector<int> stdvector = {5, 4, 3, 2, 1};
	pvector = stdvector;

	try {
		pmem::obj::transaction::run(pool, [&] {
			for (auto &e : pvector)
				e++;
			/* 6, 5, 4, 3, 2 */

			for (auto it = pvector.begin(); it != pvector.end(); it++)
				*it += 2;
			/* 8, 7, 6, 5, 4 */

			for (unsigned i = 0; i < pvector.size(); i++)
				pvector[i]--;
			/* 7, 6, 5, 4, 3 */

			std::sort(pvector.begin(), pvector.end());
			for (unsigned i = 0; i < pvector.size(); ++i)
				assert(pvector.const_at(i) == static_cast<int>(i + 3));

			pmem::obj::transaction::abort(0);
		});
	} catch (pmem::manual_tx_abort &) {
		/* expected transaction abort */
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	assert(pvector == stdvector); /* pvector element's value was rolled back */

	try {
		pmem::obj::delete_persistent<vector_type>(&pvector);
	} catch (std::exception &e) {
	}

	return 0;
}
