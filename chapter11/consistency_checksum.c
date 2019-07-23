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
  * consistency_checksum.c - example of using checksum to guarantee
  * consistency
  */

#include <assert.h>
#include <libpmemobj.h>

struct cachelines {
	char A[64];
	char B[64];
};

struct my_data {
	struct cachelines cachelines;
	uint64_t checksum;
	char padding[56];
};

uint64_t checksum_calc(struct cachelines *cachelines) {
	uint64_t checksum = 0;

	/* calculate actual checksum */

	return checksum;
}

int main()
{
	PMEMobjpool *pop = pmemobj_create("pmpool", "chksum",
					  PMEMOBJ_MIN_POOL, 0666);

	PMEMoid root = pmemobj_root(pop, sizeof(struct my_data));
	struct my_data *data = pmemobj_direct(root);

	struct my_data stack_data;
	memset(stack_data.cachelines.A, 0xC, 64);
	memset(stack_data.cachelines.B, 0xD, 64);
	stack_data.checksum = checksum_calc(&stack_data.cachelines);
	pmemobj_memcpy(pop, data, &stack_data, sizeof(stack_data),
		       PMEMOBJ_F_MEM_NONTEMPORAL);

	pmemobj_close(pop);

	return 0;
}
