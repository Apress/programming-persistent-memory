/*
 * Copyright (c) 2020, Intel Corporation
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

struct queue_node {
	pmem::obj::p<int> value;
	pmem::obj::persistent_ptr<queue_node> next;
};

struct queue {
	void
	push(pmem::obj::pool_base &pop, int value)
	{
		pmem::obj::transaction::run(pop, [&]{
			auto node = pmem::obj::make_persistent<queue_node>();
			node->value = value;
			node->next = nullptr;

			if (head == nullptr) {
				head = tail = node;
			} else {
				tail->next = node;
				tail = node;
			}
		});
	}

	int
	pop(pmem::obj::pool_base &pop)
	{
		int value;
		pmem::obj::transaction::run(pop, [&]{
			if (head == nullptr)
				throw std::out_of_range("no elements");

			auto head_ptr = head;
			value = head->value;

			head = head->next;
			pmem::obj::delete_persistent<queue_node>(head_ptr);

			if (head == nullptr)
				tail = nullptr;
		});

		return value;
	}

	void
	show()
	{
		auto node = head;
		while (node != nullptr) {
			std::cout << "show: " << node->value << std::endl;
			node = node->next;
		}

		std::cout << std::endl;
	}

private:
	pmem::obj::persistent_ptr<queue_node> head = nullptr;
	pmem::obj::persistent_ptr<queue_node> tail = nullptr;
};
