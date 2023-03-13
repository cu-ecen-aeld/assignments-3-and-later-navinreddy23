#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
//#define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
	struct thread_data* pThreadData = (struct thread_data *) thread_param;

	int rc;

	DEBUG_LOG("Thread Started: ID:%lu", *pThreadData->thread);

	usleep(pThreadData->wait_to_obtain_ms * 1000);

	rc = pthread_mutex_lock(pThreadData->mutex);
	if (rc != 0)
	{
		ERROR_LOG("[Mutex Lock]: Failed: %d", rc);
		pThreadData->thread_complete_success = false;
		return thread_param;
	}

	usleep(pThreadData->wait_to_release_ms * 1000);

	rc = pthread_mutex_unlock(pThreadData->mutex);
	if (rc != 0)
	{
		ERROR_LOG("[Mutex Unlock]: Failed: %d", rc);
		pThreadData->thread_complete_success = false;
		return thread_param;
	}

	pThreadData->thread_complete_success = true;

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

	struct thread_data* pThreadData = (struct thread_data*) malloc(1 * sizeof(struct thread_data));
	int rc = 0;

	if (pThreadData == NULL)
	{
		ERROR_LOG("Mem alloc failed.");
		return false;
	}

	pThreadData->thread = thread;
	pThreadData->mutex = mutex;
	pThreadData->wait_to_obtain_ms = wait_to_obtain_ms;
	pThreadData->wait_to_release_ms = wait_to_release_ms;

	rc = pthread_create(thread, NULL, threadfunc, (void*)pThreadData);
	if (rc != 0)
	{
		free(pThreadData);
		ERROR_LOG("[Thread Create]: Failed: %d", rc);
		return false;
	}
	else
	{
		return true;
	}

    return false;
}

