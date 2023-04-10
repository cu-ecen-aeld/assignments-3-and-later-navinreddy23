//
// Created by navin on 4/10/23.
//

#include "str_ops.h"
#include <stdio.h>
#include <sys/socket.h>

#define MAX_SEND_SIZE 1024

bool has_new_line(char* str, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		if (str[i] == '\n')
		{
			return true;
		}
	}

	return false;
}

bool write_to_file(FILE* fp, char* data, size_t data_len)
{
	int write_count = fwrite(data, sizeof (char), data_len, fp);
	return write_count == data_len;
}

void handle_send(int sock_fd, char* buffer, int send_size)
{
	int error;

	error = send(sock_fd, buffer, send_size, 0);
	if (send_size < 0)
	{
		printf("Failed to send: %d\r\n", error);
	}
}

bool read_complete_file(FILE* fp, int sock_fd)
{
	char temp[MAX_SEND_SIZE];
	int read_count = 0;
	int error;

	error = fseek(fp, 0, SEEK_SET);

	printf("Seek: %d\r\n", error);

	while(!feof(fp))
	{
		read_count = fread(temp, 1, 1024, fp);
		printf("Read  count: %d\r\n", read_count);
		handle_send(sock_fd, temp, read_count);
	}

	return true;
}
