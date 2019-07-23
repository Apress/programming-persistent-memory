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
  * consistency_flag.c - example of using flag value to gurantee consistency
  */

#include <assert.h>
#include <libpmemobj.h>

struct my_data {
	char cacheline_A[64];
	char cacheline_B[64];
	int persistent;
};

int main()
{
	PMEMobjpool *pop = pmemobj_create("pmpool", "cflag",
					  PMEMOBJ_MIN_POOL, 0666);

	PMEMoid root = pmemobj_root(pop, sizeof(struct my_data));
	struct my_data *data = pmemobj_direct(root);

	pmemobj_memset(pop, data->cacheline_A, 0xC, 64, PMEMOBJ_F_MEM_NODRAIN);
	pmemobj_memset(pop, data->cacheline_B, 0xD, 64, PMEMOBJ_F_MEM_NODRAIN);
	pmemobj_drain(pop);
	data->persistent = 1;
	pmemobj_persist(pop, &data->persistent, sizeof(data->persistent));

	pmemobj_close(pop);

	return 0;
}
