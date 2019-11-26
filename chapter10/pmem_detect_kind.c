/*
 * Copyright (c) 2019 Intel Corporation
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * pmem_detect_kind.c - illustrate the kind detection API
 */

#include <memkind.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

char path[PATH_MAX] = "/daxfs";

void memkind_fatal(int err)
{
	char error_message[MEMKIND_ERROR_MESSAGE_SIZE];

	memkind_error_message(err, error_message,
		MEMKIND_ERROR_MESSAGE_SIZE);
	fprintf(stderr, "%s\n", error_message);
	exit(1);
}

int main(int argc, char *argv[])
{
	struct memkind *pmem_kind;
	int err;
	void *buf0;
	void *buf1;

	if (argc > 2) {
		fprintf(stderr,
			"Usage: %s [pmem_kind_dir_path]\n",
			argv[0]);
		exit(1);
	} else if (argc == 2 && (realpath(argv[1], path)
		== NULL)) {
		perror(argv[1]);
		exit(1);
	}

	err = memkind_create_pmem(path, 0, &pmem_kind);
	if (err) {
		memkind_fatal(err);
	}

	/* do some allocations... */
	buf0 = memkind_malloc(pmem_kind, 1000);
	buf1 = memkind_malloc(MEMKIND_DEFAULT, 1000);

	/* look up the kind of an allocation */
	if (memkind_detect_kind(buf0) == MEMKIND_DEFAULT) {
		printf("buf0 is DRAM\n");
	} else {
		printf("buf0 is pmem\n");
	}

	err = memkind_destroy_kind(pmem_kind);
	if (err) {
		memkind_fatal(err);
	}

	exit(0);
}
