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
 * map_file_windows_example.c -- windows memory mapped file example
 *
 * This file memory-maps a file and stores a string to it.  It is
 * a quick example of how to use MapViewOfFileEx() and how to flush
 * changes to the file.
 *
 * To run this example, provide a single argument, which is the name of
 * a test file that is at least 4k in length.  This program will overwrite
 * the file contents!
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <Windows.h>

int
main(int argc, char *argv[])
{
	char *pmaddr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}

	/* Create the file or open if the file already exists */
	if ((fh = CreateFile(argv[1],
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "CreateFile, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/* Get the file length for use when memory mapping later */
	DWORD filelen = GetFileSize(fh, NULL);
	if (filelen == 0) {
		fprintf(stderr, "GetFileSize, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/* Create a file mapping object */
	HANDLE fmh = CreateFileMapping(fh,
		NULL, /* security attributes */
		PAGE_READWRITE,
		0,
		0,
		NULL);

	if (fmh == NULL) {
		fprintf(stderr, "CreateFileMapping, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/* Map into our address space and get a pointer to the beginning */
	pmaddr = (char *)MapViewOfFileEx(fmh,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		filelen,
		NULL); /* hint address */

	if (pmaddr == NULL) {
		fprintf(stderr, "MapViewOfFileEx, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/* On windows must leave the file handle(s) open while mmaped */

	/* store a string to the beginning of the file  */
	strcpy(pmaddr, "This is new data written to the file");

	/* Flush this page with length rounded up to 4k page size */
	if (FlushViewOfFile(pmaddr, 4096) == FALSE) {
		fprintf(stderr, "FlushViewOfFile, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/* Now flush the complete file to backing storage */
	if (FlushFileBuffers(fh) == FALSE) {
		fprintf(stderr, "FlushFileBuffers, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/* Explicitly unmap before closing the file */
	if (UnmapViewOfFile(pmaddr) == FALSE) {
		fprintf(stderr, "UnmapViewOfFile, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	CloseHandle(fmh);
	CloseHandle(fh);

	printf("Done.\n");
	exit(0);
}
