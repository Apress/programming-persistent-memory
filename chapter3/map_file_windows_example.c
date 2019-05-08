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
		fprintf(stderr, "CreateFileA, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/* why are we doing this????  */
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

	/* On windows we can not close file handle when file is mmaped */

	/* store a string to the Persistent Memory */
	strcpy(pmaddr, "Hello, Persistent Memory!");

	/*
	* Insure changes are flushed to storage. The length
	* needs to be rounded up to a 4k page.
	*/
	if (FlushViewOfFile(pmaddr, 4096) == FALSE) {
		fprintf(stderr, "FlushViewOfFile, gle: 0x%08x",
			GetLastError());
		exit(1);
	}

	/* Do we need this since we explicitly flushed the modified page???? */
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
