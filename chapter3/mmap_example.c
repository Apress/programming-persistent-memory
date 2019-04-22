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
 * mmap_example.c -- small, self-contained example using mmap
 *
 * This file memory-maps a file and stores a string to it.  It is
 * a quick example of how to use mmap() and msync() with Persistent
 * Memory.
 *
 * To build this example:
 * 	gcc -o mmap_example mmap_example.c
 *
 * To run it, create a 4k file, run the example, and dump the result:
 * 	dd if=/dev/zero of=testfile bs=4k count=1
 * 	od -c testfile		# see it is all zeros
 * 	./mmap_example testfile
 * 	od -c testfile		# see the result of this program
 */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int fd;
	struct stat stbuf;
	char *pmaddr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1], O_RDWR)) < 0)
		err(1, "open %s", argv[1]);

	if (fstat(fd, &stbuf) < 0)
		err(1, "stat %s", argv[1]);

	/*
	 * Map the file into our address space for read & write.
	 * Use MAP_SHARED|MAP_SYNC for pmem so stores are visible
	 * to other programs and flushing from user space is safe.
	 */
	if ((pmaddr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
					fd, 0)) == MAP_FAILED)
		err(1, "mmap %s", argv[1]);

	/* don't need the fd anymore, the mapping stays around */
	close(fd);

	/* store a string to the Persistent Memory */
	strcpy(pmaddr, "Hello, Persistent Memory!");

	/*
	 * Simplest way to flush is to call msync(). The length
	 * needs to be rounded up to a 4k page.
	 */
	if (msync((void *)pmaddr, 4096, MS_SYNC) < 0)
		err(1, "msync");

	printf("Done.\n");
	exit(0);
}
