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

 /*
 * queue.cpp -- implementation of a volatile queue.
 * create the pool for this program using pmempool, for example:
 *	pmempool create obj --layout=queue -s 1G queue_pool
 */
 
#include "persistent_queue.hpp"

enum queue_op {
	PUSH,
	POP,
	SHOW,
	EXIT,
	MAX_OPS,
};

const char *ops_str[MAX_OPS] = {"push", "pop", "show", "exit"};

queue_op
parse_queue_ops(const std::string &ops)
{
	for (int i = 0; i < MAX_OPS; i++) {
		if (ops == ops_str[i]) {
			return (queue_op)i;
		}
	}
	return MAX_OPS;
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " path_to_pool" << std::endl;
		return 1;
	}

	auto path = argv[1];
	pmem::obj::pool<queue> pool;

	try {
		pool = pmem::obj::pool<queue>::open(path, "queue");
	} catch(pmem::pool_error &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "To create pool run: pmempool create obj --layout=queue -s 100M path_to_pool" << std::endl;
	}

	auto q = pool.root();

	while (1) {
		std::cout << "[push value|pop|show|exit]" << std::endl;

		std::string command;
		std::cin >> command;

		// parse string
		auto ops = parse_queue_ops(std::string(command));

		switch (ops) {
			case PUSH: {
				int value;
				std::cin >> value;

				q->push(pool, value);

				break;
			}
			case POP: {
				std::cout << q->pop(pool) << std::endl;
				break;
			}
			case SHOW: {
				q->show();
				break;
			}
			case EXIT: {
				exit(0);
			}
			default: {
				std::cerr << "unknown ops" << std::endl;
				exit(0);
			}
		}
	}
}
