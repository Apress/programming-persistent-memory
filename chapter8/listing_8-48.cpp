/*
Copyright (c) 2019, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * listing_8-48.cpp -- checking the consistency of the data structure written
 *                     by listing_8-44.cpp
 */

#include <stdio.h>
#include <stdint.h>
#include <libpmemobj++/persistent_ptr.hpp>

using namespace std;
namespace pobj = pmem::obj;

struct header_t {
    uint32_t counter;
    uint8_t reserved[60];
};
struct record_t {
    char name[63];
    char valid;
};
struct root {
    pobj::persistent_ptr<header_t> header;
    pobj::persistent_ptr<record_t[]> records;
};

pobj::pool<root> pop;

int main(int argc, char *argv[]) {

    pop = pobj::pool<root>::open("/mnt/pmem/file", "RECORDS");
    auto proot = pop.root();
    pobj::persistent_ptr<header_t> header = proot->header;
    pobj::persistent_ptr<record_t[]> records = proot->records;

    for (uint8_t i = 0; i < header->counter; i++) {
        if (records[i].valid < 1 or records[i].valid > 2)
            return 1; // we should not have reached this record
    }

    pop.close();
    return 0; // everything ok
}
