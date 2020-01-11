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

/*
 * listing_14-2.cpp -- listing_14-2.cpp demonstrates that PMDK transactions
 *                     do not support isolation
 */

#include <iostream>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/mutex.hpp>
#include <thread>
#include <mutex> // for std::unique_lock
#include <vector>

using namespace std;
namespace pobj = pmem::obj;

struct root {
    pobj::mutex mtx;
    pobj::p<int> counter;
};

using pop_type = pobj::pool<root>;

void increment(pop_type &pop) {
    auto proot = pop.root();
    pobj::transaction::run(pop, [&] {
        std::unique_lock<pobj::mutex> lock(proot->mtx); 
        proot->counter.get_rw() += 1;
    });
}

int main(int argc, char *argv[]) {
    pop_type pop =
        pop_type::open("/daxfs/file", "COUNTER_INC");

    auto proot = pop.root();

    cout << "Counter = " << proot->counter << endl;

    std::vector<std::thread> workers;
    workers.reserve(10);
    for (int i = 0; i < 10; ++i) {
        workers.emplace_back(increment, std::ref(pop));
    }

    for (int i = 0; i < 10; ++i) {
        workers[i].join();
    }

    cout << "Counter = " << proot->counter << endl;

    pop.close();
    return 0;
}
