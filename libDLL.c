#include <fcntl.h>
#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus

extern "C" {
#endif

char* read_func(int fd, int filesize) {
	char *buffer;
	struct aiocb aiocb_struct;
	memset(&aiocb_struct, 0, sizeof(aiocb_struct));
	buffer = (char*)calloc(filesize,sizeof(char));
	aiocb_struct.aio_fildes = fd;
	aiocb_struct.aio_offset = 0;
	aiocb_struct.aio_buf = buffer;
	aiocb_struct.aio_nbytes = filesize - 1;
	if(aio_read(&aiocb_struct)){
		perror("aio_read");
		exit(1);
	}
	while(aio_error(&aiocb_struct) != 0){}
	return buffer;
}

void write_func(int fd, int filesize, int OFFSET, char* buffer) {
	struct aiocb aiocb_struct;
	memset(&aiocb_struct, 0, sizeof(aiocb_struct));
	aiocb_struct.aio_fildes = fd;
	aiocb_struct.aio_offset = OFFSET;
	aiocb_struct.aio_buf = buffer;
	aiocb_struct.aio_nbytes = filesize;
	if(aio_write(&aiocb_struct)){
		perror("aio_write");
		exit(1);
	}
	while(aio_error(&aiocb_struct) != 0){}
	return;
}

#ifdef __cplusplus
}
#endif

