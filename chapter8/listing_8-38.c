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
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <valgrind/pmemcheck.h>
void flush (const void *addr, size_t len) {
	uintptr_t flush_align = 64, uptr;
	for (uptr = (uintptr_t)addr & ~(flush_align -1);
		uptr < (uintptr_t)addr + len; uptr += flush_align)
		_mm_clflush ((char *)uptr); 
}
int main (int argc, char *argv[]) {
	int fd, *data;
	fd = open ("/mnt/pmem/file", O_CREAT|O_RDWR, 0666);
	posix_fallocate (fd, 0, sizeof (int));
	data = (int *)mmap (NULL, sizeof (int), PROT_READ|PROT_WRITE, 
                            MAP_SHARED_VALIDATE | MAP_SYNC, fd, 0);
	VALGRIND_PMC_REGISTER_PMEM_MAPPING (data, sizeof (int));
	*data = 1234;
	flush ((void *)data, sizeof (int));
	flush ((void *)data, sizeof (int));
	munmap (data, sizeof(int));
	VALGRIND_PMC_REMOVE_PMEM_MAPPING (data, sizeof (int));
	return 0;
}
