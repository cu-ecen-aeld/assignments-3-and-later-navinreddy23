#include "systemcalls.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char* cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
	int retVal = 0;

	retVal = system(cmd);

	if (retVal != 0)
	{
		return false;
	}

	return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
	va_list args;
	va_start(args, count);
	char * command[count+1];
	int i;
	int status = 0;

	for(i=0; i<count; i++)
	{
		command[i] = va_arg(args, char *);
	}
	command[count] = NULL;

	va_end(args);

	fflush(stdout);

	pid_t pid = fork();
	if(pid == 0)
	{
		execv(command[0], command);

		//! Kill the child process
		exit(-1);
	}
	else if(pid > 0)
	{
		if(waitpid(pid, &status, 0) < 0)
		{
			return false;
		}

		if(WEXITSTATUS(status))
		{
			return false;
		}

		return true;
	}
	else
	{
		printf("Error in fork\n");
		return false;
	}



	return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char* outputfile, int count, ...)
{
	va_list args;
	va_start(args, count);
	char* command[count + 1];
	int i;
	for (i         = 0; i < count; i++)
	{
		command[i] = va_arg(args, char *);
	}
	command[count] = NULL;

	va_end(args);

	int   status;
	pid_t pid;

	int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
	if (fd < 0)
	{
		perror("File open");
		return false;
	}

	pid = fork();
	if (pid == -1)
	{
		return false;
	}
	else if (pid == 0)  //! Child process
	{
		if (dup2(fd, 1) < 0)
		{
			perror("dup2");
			return false;
		}

		execv(command[0], command);

		close(fd);
		exit(1);
	}
	else
	{
		if(waitpid(pid, &status, 0) < 0)
		{
			close(fd);
			return false;
		}

		if(WEXITSTATUS(status))
		{
			close(fd);
			return false;
		}

		close(fd);
		return true;
	}
}
