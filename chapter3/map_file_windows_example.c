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
	HANDLE fh = INVALID_HANDLE_VALUE;
	if ((fh = CreateFileA(argv[1],
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "CreateFileA, gle: 0x%08x",
			GetLastError());
		exit(1);
	}
	DWORD filelen = GetFileSize(fh, NULL);
	if (filelen == 0) {
		fprintf(stderr, "GetFileSize, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/*
	 * Map the file into our address space for read & write.
	 * Use MAP_SHARED|MAP_SYNC for pmem so stores are visible
	 * to other programs and flushing from user space is safe.
	 */
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

	/* don't need the fd anymore, the mapping stays around */
	// close(fd); on windows we can not close file handle when file is mmaped

	/* store a string to the Persistent Memory */
	strcpy(pmaddr, "Hello, Persistent Memory!");

	/*
	* Simplest way to flush is to call msync(). The length
	* needs to be rounded up to a 4k page.
	*/
	if (FlushViewOfFile(pmaddr, 4096) == FALSE) {
		fprintf(stderr, "FlushViewOfFile, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	if (FlushFileBuffers(fh) == FALSE) {
		fprintf(stderr, "FlushFileBuffers, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	printf("Done.\n");

	if (UnmapViewOfFile(pmaddr) == FALSE) {
		fprintf(stderr, "UnmapViewOfFile, gle: 0x%08x", GetLastError());
		exit(1);
	}

	CloseHandle(fmh);
	CloseHandle(fh);

	exit(0);
}