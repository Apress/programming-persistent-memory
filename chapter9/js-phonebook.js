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
 * js-phonebook.js -- phonebook example with pmemkv JavaScript bindings
 */

const Database = require('./lib/all');

function assert(condition) {
    if (!condition) throw new Error('Assert failed');
}

console.log('Create a key-value store using the "cmap" engine');
const db = new Database('cmap', '{"path":"/daxfs/kvfile", "size":1073741824, "force_create":1}');

console.log('Add 2 entries with name and phone number');
db.put('John', '123-456-789');
db.put('Kate', '987-654-321');

console.log('Count elements');
assert(db.count_all == 2);

console.log('Read key back');
assert(db.get('John') === '123-456-789');

console.log('Iterate through the phonebook');
db.get_all((k, v) => console.log(`name: ${k}, number: ${v}`));

console.log('Remove one record');
db.remove('John');

console.log('Lookup of removed record');
assert(!db.exists('John'));

console.log('Stopping engine');
db.stop();
