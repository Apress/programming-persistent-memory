/*
 * Copyright (c) 2019, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of Intel Corporation nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <emmintrin.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/transaction.hpp>
#include <valgrind/pmemcheck.h>

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

int main (int argc, char *argv[]) {
	
	VALGRIND_PMC_EMIT_LOG("PMREORDER_TAG.BEGIN");

	pop = pobj::pool<root>::open ("/mnt/pmem/file", "RECORDS");
	auto proot = pop.root ();

	pobj::transaction::run (pop, [&] {
		proot->header = pobj::make_persistent<header_t> ();
		proot->header->counter = 0;
		proot->records = pobj::make_persistent<record_t[]> (1);
		proot->records[0].valid = 0;
	});	
	pobj::persistent_ptr<header_t> header  = proot->header;
	pobj::persistent_ptr<record_t[]> records = proot->records;

	VALGRIND_PMC_EMIT_LOG("PMREORDER_TAG.END");

	header->counter = 0;
	for (uint8_t i = 0; i < 10; i++) {
		if (rand() % 2 == 0) {
			snprintf (records[i].name, 63, "record #%u", i + 1);
			pop.persist (records[i].name, 63);
			records[i].valid = 2;
		} else
			records[i].valid = 1;
		pop.persist (&(records[i].valid), 1);
		header->counter++;
	}
	pop.persist (&(header->counter), 4);

	pop.close ();
	return 0;
}
