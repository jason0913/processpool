/*
* @Author: jql
* @Date:   2016-01-14 21:42:49
* @Last Modified by:   jql
* @Last Modified time: 2016-01-16 22:18:13
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "logger.h"
#include "sockopt.h"
#include "define.h"
#include "child.h"
#include "global.h"

int main(int argc, char const *argv[])
{
	char bind_addr[IPADDR_SIZE];
	socklen_t addrlen;
	char buf[1024];

	int listensock;
	int port = 30000;
	int child_nums = 4;
	int child_idle_num = child_nums;
	int isfind = 0;
	int result;

	memset(bind_addr,0,sizeof(bind_addr));
	memset(buf,0,sizeof(buf));
	addrlen = sizeof(struct sockaddr_in);

	listensock = socketServer(bind_addr,port,PROCESSPOOL_ERROR_LOG_FILENAME);
	if (listensock <0)
	{
		printf("socketServer failed!\
			file:%s,line:%d\n",__FILE__,__LINE__);
		return 5;
	}

	result = create_childs(listensock,child_nums,addrlen,PROCESSPOOL_ERROR_LOG_FILENAME);
	if (result <0)
	{
		printf("create_childs failed!\
			file:%s,line:%d\n",__FILE__,__LINE__);
		return 6;
	}

	int maxfd;
	fd_set read_set;
	fd_set exception_set;
	FD_ZERO(&read_set);
	FD_ZERO(&exception_set);

	struct timeval t;

	while(1)
	{
		t.tv_sec =10;
		t.tv_usec = 0;
		FD_SET(listensock,&read_set);
		FD_SET(listensock,&exception_set);

		int i;
		maxfd = child_process[0].pipefd[0];
		for (i = 0; i < child_nums; ++i)
		{
			FD_SET(child_process[i].pipefd[0],&read_set);
			if (child_process[i].pipefd[0] > maxfd)
			{
				maxfd = child_process[i].pipefd[0];
			}
		}

		maxfd = (maxfd > listensock ? maxfd:listensock) +1;

		result = select(maxfd,&read_set,NULL,&exception_set,&t);
		if (result < 0)
		{
			printf("create_childs failed!\
				file:%s,line:%d\n",__FILE__,__LINE__);
			return 6;
		}
		if (0 == result)
		{
			printf("select timeout\n");
			continue;
		}
		if (FD_ISSET(listensock,&read_set))
		{
			for (i = 0; i < child_nums; ++i)
			{
				if (child_process[i].status == IDLE_STATUS)
				{
					isfind = 1;
					break;
				}
			}
			if (0 == isfind)
			{
				// create_more_childs();
			}
			child_process[i].status = BUSY_STATUS;
			child_idle_num--;

			sprintf(buf,"%s","new");
			result = write(child_process[i].pipefd[0],buf,sizeof(buf));
		}

		for (i = 0; i < child_nums; ++i)
		{
			if (FD_ISSET(child_process[i].pipefd[0],&read_set))
			{
				memset(buf,0,sizeof(buf));
				result = read(child_process[i].pipefd[0],buf,sizeof(buf));
				printf("child %d: finish,message is %s\n", i,buf);
				sleep(1);

				child_process[i].status = IDLE_STATUS;
				child_idle_num++;
			}
		}
		if (child_idle_num > MAX_IDLE_NUM)
		{
			// adjust_child_num();
		}
	}
	return 0;
}