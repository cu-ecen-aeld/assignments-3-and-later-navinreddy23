#include <stdio.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#define EXPECTED_ARG_COUNT 3

int main(int argc, char** argv)
{
	FILE* fd;
	int error;
	const char * pFileName = argv[1];
	const char* pWriteStr = argv[2];

	openlog("[Writer]", LOG_NDELAY, LOG_USER);

	syslog(LOG_DEBUG, "Argc: %d", argc);

	if (argc != EXPECTED_ARG_COUNT)
	{
		syslog(LOG_ERR, "Insufficient arguments. Expected 3 got %d", argc);
		syslog(LOG_DEBUG, "Usage ./writer FILENAME WRITE_STR");
		return 1;
	}

	fd = fopen(pFileName, "w");
	error = errno;
	if(fd == NULL)
	{
		syslog(LOG_ERR, "Failed to create file in write mode: %s. Reason: %s", argv[1], strerror(error));
		return 1;
	}

	error = fwrite(pWriteStr, strlen(pWriteStr), 1, fd);
	if(error <= 0)
	{
		syslog(LOG_ERR, "Failed to write to file: %s", strerror(error));
		return 1;
	}
	else
	{
		syslog(LOG_DEBUG, "Wrote: %s to %s", pWriteStr, pFileName);
	}

	//! Append new line
	error = fwrite("\r\n", strlen("\r\n"), 1, fd);
	if(error <= 0)
	{
		syslog(LOG_ERR, "Failed to append new line: %s", strerror(error));
		return 1;
	}

	closelog();
	fclose(fd);

	return 0;
}