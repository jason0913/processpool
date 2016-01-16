#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#define MAX_PATH_SIZE	256
#define LINE_MAX    1024
#define IPADDR_SIZE 16
#define PROCESSPOOL_ERROR_LOG_FILENAME      "processpool"
//#define MAX_PROCESS_NUMBER 16
#define MAX_PROCESS_NUMBER 4
#define MAX_IDLE_NUM 10

#define IDLE_STATUS  0
#define BUSY_STATUS 1

#include <unistd.h>

struct sub_process
{
	pid_t child_pid;
	int  status;
	int pipefd[2];
};

typedef struct sub_process processpool;

#endif