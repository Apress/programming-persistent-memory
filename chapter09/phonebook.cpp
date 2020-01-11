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
 * phonebook.cpp -- phonebook example with pmemkv
 */

#include <iostream>
#include <cassert>
#include <libpmemkv.hpp>
#include <string>
#include "pmemkv_config.h"

using namespace pmem::kv;

auto PATH = "/daxfs/kvfile";
const uint64_t FORCE_CREATE = 1;
const uint64_t SIZE = 1024 * 1024 * 1024;  // 1 Gig

int main() {
    // Prepare config for pmemkv database
    pmemkv_config *cfg = config_setup(PATH, FORCE_CREATE, SIZE);
    assert(cfg != nullptr);

    // Create a key-value store using the "cmap" engine.
    db kv;

    if (kv.open("cmap", config(cfg)) != status::OK) {
        std::cerr << db::errormsg() << std::endl;
        return 1;
    }

    // Add 2 entries with name and phone number
    if (kv.put("John", "123-456-789") != status::OK) {
        std::cerr << db::errormsg() << std::endl;
        return 1;
    }
    if (kv.put("Kate", "987-654-321") != status::OK) {
        std::cerr << db::errormsg() << std::endl;
        return 1;
    }

    // Count elements
    size_t cnt;
    if (kv.count_all(cnt) != status::OK) {
        std::cerr << db::errormsg() << std::endl;
        return 1;
    }
    assert(cnt == 2);

    // Read key back
    std::string number;
    if (kv.get("John", &number) != status::OK) {
        std::cerr << db::errormsg() << std::endl;
        return 1;
    }
    assert(number == "123-456-789");

    // Iterate through the phonebook
    if (kv.get_all([](string_view name, string_view number) {
            std::cout << "name: " << name.data() <<
            ", number: " << number.data() << std::endl;
            return 0;
            }) != status::OK) {
        std::cerr << db::errormsg() << std::endl;
        return 1;
    }

    // Remove one record
    if (kv.remove("John") != status::OK) {
        std::cerr << db::errormsg() << std::endl;
        return 1;
    }

    // Look for removed record
    assert(kv.exists("John") == status::NOT_FOUND);

    // Try to use one of methods of ordered engines
    assert(kv.get_above("John", [](string_view key, string_view value) {
        std::cout << "This callback should never be called" << std::endl;
        return 1;
    }) == status::NOT_SUPPORTED);

    // Close database (optional)
    kv.close();

    return 0;
}
