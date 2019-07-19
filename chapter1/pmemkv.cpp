/*
 * Copyright (c) 2019, Intel Corporation
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
 * pmemkv.cpp -- demo a persistent memory key-value store
 */

#include <iostream>
#include <cassert>
#include <libpmemkv.hpp>

using namespace pmem::kv;
using std::cout;
using std::endl;
using std::string;

auto PATH = "/daxfs/kvfile";

const uint64_t SIZE = 1024 * 1024 * 1024;  // 1 Gig

/*
 * kvprint -- print a single key-value pair
 */
int kvprint(string_view k, string_view v) {
    cout << "key: " << k.data() << ", value: " << v.data() << endl;
    return 0;
}

int main() {

    pmemkv_config *cfg = pmemkv_config_new();
    assert(cfg != nullptr);

    int ret = pmemkv_config_put_string(cfg, "path", PATH);
    assert(ret == PMEMKV_STATUS_OK);

    ret = pmemkv_config_put_uint64(cfg, "force_create", 1);
    assert(ret == PMEMKV_STATUS_OK);

    ret = pmemkv_config_put_uint64(cfg, "size", SIZE);
    assert(ret == PMEMKV_STATUS_OK);

    // Create a key-value store using the "cmap" engine.
    db *kv = new db();
    assert(kv != nullptr);

    status s = kv->open("cmap", cfg);
    assert(s == status::OK);

    // add some keys and values
    s = kv->put("key1", "value1");
    assert(s == status::OK);
    s = kv->put("key2", "value2");
    assert(s == status::OK);
    s = kv->put("key3", "value3");
    assert(s == status::OK);

    // iterate through the key-value store
    kv->get_all(kvprint);

    // Stop the engine.
    delete kv;

    return 0;
}
