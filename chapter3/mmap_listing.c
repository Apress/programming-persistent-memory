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
	 * Use MAP_SHARED so stores are visible to other programs.
	 */
	if ((pmaddr = mmap(NULL, stbuf.st_size,
				PROT_READ|PROT_WRITE,
				MAP_SHARED, fd, 0)) == MAP_FAILED)
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
