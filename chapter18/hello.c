/*
 * Copyright 2020, Intel Corporation
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
 * hello.c -- demonstrate API for librpmem
 */

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <librpmem.h>

/*
 * English and Spanish translation of the message
 */
enum lang_t {en, es};
static const char *hello_str[] = {
	[en] = "Hello world!",
	[es] = "¡Hola Mundo!"
};

/*
 * structure to store the current message
 */
#define STR_SIZE	100
struct hello_t {
	enum lang_t lang;
	char str[STR_SIZE];
};

/*
 * write_hello_str -- write a message to the local memory
 */
static inline void
write_hello_str(struct hello_t *hello, enum lang_t lang)
{
	hello->lang = lang;
	strncpy(hello->str, hello_str[hello->lang], STR_SIZE);
}

/*
 * remote_open -- setup the librpmem replication
 */
static inline RPMEMpool*
remote_open(const char *target, const char *poolset, void *pool,
		size_t pool_size, int *created)
{
	/* fill pool_attributes */
	struct rpmem_pool_attr pool_attr;
	memset(&pool_attr, 0, sizeof(pool_attr));
	strncpy(pool_attr.signature, "HELLO", RPMEM_POOL_HDR_SIG_LEN);

	/* create a remote pool */
	unsigned nlanes = 1;
	RPMEMpool *rpp = rpmem_create(target, poolset, pool, pool_size, &nlanes,
			&pool_attr);
	if (rpp) {
		*created = 1;
		return rpp;
	}

	/* create failed so open a remote pool */
	assert(errno == EEXIST);
	rpp = rpmem_open(target, poolset, pool, pool_size, &nlanes, &pool_attr);
	assert(rpp != NULL);
	*created = 0;

	return rpp;
}

int
main(int argc, char *argv[])
{
	/* for this example, assume 32MiB pool */
	size_t pool_size = 32 * 1024 * 1024;
	void *pool = NULL;
	int created;
	
	/* allocate a page size aligned local memory pool */
	long pagesize = sysconf(_SC_PAGESIZE);
	assert(pagesize >= 0);
	int ret = posix_memalign(&pool, pagesize, pool_size);
	assert(ret == 0 && pool != NULL);

	/* skip to the beginning of the message */
	size_t hello_off = 4096; /* rpmem header size */
	struct hello_t *hello = (struct hello_t *)(pool + hello_off);

	RPMEMpool *rpp = remote_open("target", "pool.set", pool, pool_size,
			&created);
	if (created) {
		/* reset local memory pool */
		memset(pool, 0, pool_size);
		write_hello_str(hello, en);
	} else {
		/* read message from the remote pool */
		ret = rpmem_read(rpp, hello, hello_off, sizeof(*hello), 0);
		assert(ret == 0);

		/* translate the message */
		const int lang_num = (sizeof(hello_str) / sizeof(hello_str[0]));
		enum lang_t lang = (enum lang_t)((hello->lang + 1) % lang_num);
		write_hello_str(hello, lang);
	}

	/* write message to the remote pool */
	ret = rpmem_persist(rpp, hello_off, sizeof(*hello), 0, 0);
	printf("%s\n", hello->str);
	assert(ret == 0);

	/* close the remote pool */
	ret = rpmem_close(rpp);
	assert(ret == 0);

	/* release local memory pool */
	free(pool);
	return 0;
}
